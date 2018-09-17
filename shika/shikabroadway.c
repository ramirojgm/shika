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

typedef struct
{
  GPid host_pid;
  guint16 host_sid;
  gboolean redirecting;
} ShikaBroadwayPrivate;

typedef struct
{
  GOutputStream * output;
  GInputStream * input;
} ShikaBroadwayProxy;

G_DEFINE_TYPE_WITH_PRIVATE(ShikaBroadway,shika_broadway,G_TYPE_OBJECT)

static void
shika_broadway_init(ShikaBroadway * broadway)
{
  ShikaBroadwayPrivate * priv = shika_broadway_get_instance_private(broadway);
  g_mutex_init(&broadway->mutex);
  priv->host_sid = 0;
  priv->host_pid = 0;
  priv->redirecting = FALSE;
}

ShikaBroadway *
shika_broadway_new()
{
  return SHIKA_BROADWAY(g_object_new(SHIKA_TYPE_BROADWAY,NULL));
}


static void
_shika_broadway_watch_pid (GPid     pid,
			   gint     status,
			   gpointer user_data)
{
  ShikaBroadway * broadway = SHIKA_BROADWAY(user_data);
  g_mutex_lock(&broadway->mutex);
  ShikaBroadwayPrivate * priv = shika_broadway_get_instance_private(broadway);
  g_autofree gchar * socket_address =
      g_strdup_printf("shika-host%d.socket",priv->host_sid);

  priv->host_sid = 0;
  priv->host_pid = 0;
  priv->redirecting = FALSE;

  if(g_file_test(socket_address,G_FILE_TEST_EXISTS))
    g_unlink(socket_address);

  g_mutex_unlock(&broadway->mutex);
  g_spawn_close_pid (pid);
}

gboolean
shika_broadway_is_running(ShikaBroadway * broadway)
{
  gboolean result = 0;
  g_mutex_lock(&broadway->mutex);
  ShikaBroadwayPrivate * priv = shika_broadway_get_instance_private(broadway);
  result = priv->host_pid != 0;
  g_mutex_unlock(&broadway->mutex);
  return result;
}

gboolean
shika_broadway_run(ShikaBroadway * broadway,
		   const gchar * program_path,
		   const gchar * arguments,
		   GError ** error)
{
  gboolean done = 0;
  static volatile guint display = 1024;
  display ++;

  g_autofree gchar * host_display = g_strdup_printf("%d",display);

  const gchar * const
    service_argv[] = {
	"./bin/shika-host",
	host_display,
	program_path,
	arguments,
	NULL };

  GPid service_pid = 0;

  if((done = g_spawn_async(NULL,(gchar**) service_argv,
				       NULL,
				       G_SPAWN_DO_NOT_REAP_CHILD,
				       NULL,NULL, &service_pid, error)))
    {
      g_child_watch_add(service_pid,_shika_broadway_watch_pid,broadway);
      g_mutex_lock(&broadway->mutex);
      ShikaBroadwayPrivate * priv =
	  shika_broadway_get_instance_private(broadway);

      priv->host_pid = service_pid;
      priv->host_sid = display;
      g_mutex_unlock(&broadway->mutex);
      g_usleep(100000);
    }
  return done;
}

static gpointer
_shika_broadway_proxy_thread(gpointer data)
{
  ShikaBroadwayProxy * proxy = (ShikaBroadwayProxy *)data;
  GInputStream * input = proxy->input;
  GOutputStream * output = proxy->output;
  gchar byte = 0;
  while (g_input_stream_read(input,&byte,1,NULL,NULL) > 0)
      if(g_output_stream_write(output,&byte,1,NULL,NULL) <= 0)
	break;
  return NULL;
}

gboolean
shika_broadway_redirect(ShikaBroadway * broadway,
			HttpRequest * request,
			GIOStream * stream,
			GError ** error)
{

  gboolean done = TRUE;
  g_mutex_lock(&broadway->mutex);

  ShikaBroadwayPrivate * priv = shika_broadway_get_instance_private(broadway);

  g_autoptr(GSocketConnection) connection = NULL;
  g_autofree gchar * host_address = g_strdup_printf("shika-host%d.socket",
						    priv->host_sid);

  g_autoptr(GSocketAddress) address =
    g_unix_socket_address_new(host_address);

  GSocket * socket = g_socket_new(G_SOCKET_FAMILY_UNIX,
				  G_SOCKET_TYPE_STREAM,
				  G_SOCKET_PROTOCOL_DEFAULT,
				  error);
  g_socket_set_keepalive(socket,TRUE);
  g_socket_set_keepalive(
      g_socket_connection_get_socket(G_SOCKET_CONNECTION(stream)),TRUE);
  g_mutex_unlock(&broadway->mutex);

  if((done = g_socket_connect(socket,address,NULL,error)))
    {
      connection = g_socket_connection_factory_create_connection(socket);
      done = http_package_write_to_stream(
                 HTTP_PACKAGE(request),
                 g_io_stream_get_output_stream(G_IO_STREAM(connection)),
                 NULL,
                 NULL,
                 error);
    }
  else
    {
      g_object_unref(socket);
    }

  if(done)
    {
      ShikaBroadwayProxy input_proxy = {
        g_io_stream_get_output_stream(G_IO_STREAM(connection)),
        g_io_stream_get_input_stream(stream),
      };

      ShikaBroadwayProxy output_proxy = {
        g_io_stream_get_output_stream(stream),
        g_io_stream_get_input_stream(G_IO_STREAM(connection)),
      };

      GThread * input_thread =
          g_thread_new(NULL,_shika_broadway_proxy_thread,&input_proxy);

      GThread * output_thread =
          g_thread_new(NULL,_shika_broadway_proxy_thread,&output_proxy);

      g_mutex_lock(&broadway->mutex);
      priv->redirecting = TRUE;
      g_mutex_unlock(&broadway->mutex);

      g_usleep(2000);

      g_thread_join(input_thread);
      g_thread_join(output_thread);

      g_mutex_lock(&broadway->mutex);
      priv->redirecting = FALSE;
      g_mutex_unlock(&broadway->mutex);

      g_io_stream_close(G_IO_STREAM(connection),NULL,NULL);
      g_socket_close(socket,NULL);
    }
  return done;
}

void
shika_broadway_kill(ShikaBroadway * broadway)
{
  g_mutex_lock(&broadway->mutex);
  ShikaBroadwayPrivate * priv = shika_broadway_get_instance_private(broadway);
  g_autofree gchar * socket_address =
        g_strdup_printf("shika-host%d.socket",priv->host_sid);

  if(priv->host_sid)
    {
      kill(priv->host_pid,SIGTSTP);
      kill(priv->host_pid,SIGQUIT);
      kill(priv->host_pid,SIGINT);
      if(g_file_test(socket_address,G_FILE_TEST_EXISTS))
	  g_unlink(socket_address);
    }
  g_mutex_unlock(&broadway->mutex);
}

static void
shika_broadway_class_init(ShikaBroadwayClass * broadway)
{

}
