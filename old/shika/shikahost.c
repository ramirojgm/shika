/*
	Copyright (C) 2018 Ramiro Jose Garcia Moraga

	This file is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with the this software.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <shika.h>
#include <gio-unix-2.0/gio/gunixsocketaddress.h>
#include <glib/gstdio.h>
#include <signal.h>

struct _ShikaHostPrivate
{
  GSocketAddress * address;
  guint display;

  GPid host_pid;
  GPid client_pid;

  GSocketConnection * remote;
  GSocketConnection * local;
  ShikaPipe * remote_pipe;
  ShikaPipe * local_pipe;
};

G_DEFINE_TYPE_WITH_PRIVATE(ShikaHost,shika_host,G_TYPE_OBJECT)

static void
shika_host_init(ShikaHost * self)
{
  static guint display = 1023;
  static gchar temp_dir[256] = {0,};
  static GMutex mutex = G_STATIC_MUTEX_INIT;

  g_mutex_lock(&mutex);
  display ++;

  g_autofree gchar * unix_path = NULL;
  g_autofree gchar * filename = g_strdup_printf("shika_host%d.socket",display);

  if(temp_dir[0] == 0)
    {
      g_autofree gchar * tmp = g_dir_make_tmp(NULL,NULL);
      strcpy(temp_dir,tmp);
    }

  unix_path = g_build_filename(temp_dir,filename,NULL);

  self->priv = shika_host_get_instance_private(self);
  self->priv->address = g_unix_socket_address_new(unix_path);
  self->priv->display = display;

  g_mutex_unlock(&mutex);
}

ShikaHost *
shika_host_new()
{
  return SHIKA_HOST(g_object_new(SHIKA_TYPE_HOST,NULL));
}

const GSocketAddress *
shika_host_get_address(ShikaHost * host)
{
  return host->priv->address;
}

gboolean
shika_host_is_connected(ShikaHost * host)
{
  return host->priv->remote != NULL;
}

gboolean
shika_host_is_running(ShikaHost * host)
{
  return host->priv->host_pid != 0;
}

static void
shika_host_on_pipe_close(ShikaPipe * pipe,gpointer data)
{
  ShikaHost * self = SHIKA_HOST(data);

  g_clear_object(&self->priv->local_pipe);
  g_clear_object(&self->priv->remote_pipe);

  g_clear_object(&self->priv->local);
  g_clear_object(&self->priv->remote);
}


gboolean
shika_host_pipe(ShikaHost * host,
		HttpRequest * request,
		GSocketConnection * remote,
		GError ** error)
{
  g_return_val_if_fail(shika_host_is_connected(host) == FALSE,FALSE);
  g_return_val_if_fail(shika_host_is_running(host),FALSE);

  gboolean done = FALSE;
  GSocket * socket = NULL;

  done = (socket = g_socket_new(G_SOCKET_FAMILY_UNIX,
    				  G_SOCKET_TYPE_STREAM,
    				  G_SOCKET_PROTOCOL_DEFAULT,
    				  error)) != NULL;
  if((done = g_socket_connect(socket,host->priv->address,NULL,error)))
    {
      host->priv->local = g_socket_connection_factory_create_connection(socket);
      host->priv->remote = G_SOCKET_CONNECTION(g_object_ref(remote));

      done = http_package_write_to_stream(
                 HTTP_PACKAGE(request),
                 g_io_stream_get_output_stream(G_IO_STREAM(host->priv->local)),
                 NULL,
                 NULL,
                 error);

      host->priv->local_pipe = shika_pipe_new();
      host->priv->remote_pipe = shika_pipe_new();

      g_signal_connect(host->priv->local_pipe,
		       "closed",
		       G_CALLBACK(shika_host_on_pipe_close),
		       host);

      g_signal_connect(host->priv->remote_pipe,
      		       "closed",
      		       G_CALLBACK(shika_host_on_pipe_close),
      		       host);

      shika_pipe_open(
	  host->priv->local_pipe,
	  g_io_stream_get_input_stream(G_IO_STREAM(host->priv->local)),
	  g_io_stream_get_output_stream(G_IO_STREAM(host->priv->remote)));

      shika_pipe_open(
	  host->priv->remote_pipe,
	  g_io_stream_get_input_stream(G_IO_STREAM(host->priv->remote)),
	  g_io_stream_get_output_stream(G_IO_STREAM(host->priv->local)));
    }

  return done;
}

static void
_shika_host_watch_pid(GPid pid,
		      gint status,
		      gpointer user_data)
{
  ShikaHost * host = SHIKA_HOST(user_data);
  shika_host_kill(host);
}

gboolean
shika_host_run(ShikaHost * host,
	       const gchar * program_path,
	       const gchar * arguments,
	       GError ** error)
{
  gboolean done = FALSE;

  g_autofree gchar * host_display =
      g_strdup_printf(":%d",host->priv->display);

  g_autofree gchar * client_commandline =
      g_strdup_printf("export GDK_BACKEND=broadway && export BROADWAY_DISPLAY=:%d && export GTK_THEME=Adwaita && %s %s",
		      host->priv->display,
		      program_path,
		      arguments);

  const gchar * host_address =
      g_unix_socket_address_get_path(
	  G_UNIX_SOCKET_ADDRESS(host->priv->address));

  const gchar *
  host_argv[] = {
      "/usr/bin/broadwayd",
      host_display,
      "-u",host_address,
      NULL
  };

  const gchar *
  client_argv[] = {
      "/bin/sh",
      "-c",client_commandline,NULL
  };


  if((done = g_spawn_async(NULL, (gchar**)host_argv,
				 NULL,
				 G_SPAWN_DO_NOT_REAP_CHILD,
				 NULL,NULL, &host->priv->host_pid, error)))
    {
      g_child_watch_add(host->priv->host_pid,_shika_host_watch_pid,host);
      g_usleep(10000);
      if((done = g_spawn_async(NULL, (gchar**)client_argv,
			      NULL,
			      G_SPAWN_DO_NOT_REAP_CHILD,
			      NULL,
			      NULL,
			      &host->priv->client_pid,
			      error)))
	{
	  g_child_watch_add(host->priv->client_pid,_shika_host_watch_pid,host);
	  g_print("Done");
	}
    }
  return done;
}

void
shika_host_kill(ShikaHost * host)
{
  kill(host->priv->client_pid,SIGKILL);
  kill(host->priv->host_pid,SIGKILL);

  if(host->priv->local)
    g_io_stream_close(G_IO_STREAM(host->priv->local),NULL,NULL);

  g_clear_object(&host->priv->local_pipe);
  g_clear_object(&host->priv->local);

  if(host->priv->remote)
    g_io_stream_close(G_IO_STREAM(host->priv->remote),NULL,NULL);

  g_clear_object(&host->priv->remote_pipe);
  g_clear_object(&host->priv->remote);

}

static void
shika_host_dispose(GObject * object)
{}

static void
shika_host_class_init(ShikaHostClass * klass)
{
  G_OBJECT_CLASS(klass)->dispose = shika_host_dispose;
}
