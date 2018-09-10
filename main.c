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

static gboolean
shika_filesystem_run(GSocketService * service,
		  GSocketConnection * connection,
		  GObject * source,
		  gpointer data);

static gboolean
shika_service_run(GSocketService * service,
		  GSocketConnection * connection,
		  GObject * source,
		  gpointer data);

int
main(int argc,char ** argv)
{
  g_autoptr(GMainLoop) main_loop = g_main_loop_new(NULL,FALSE);
  g_autoptr(GSocketService) service = g_threaded_socket_service_new(100);


  g_socket_listener_add_inet_port(G_SOCKET_LISTENER(service),
				  7080,
				  NULL,
				  NULL);

  g_signal_connect(G_OBJECT(service),"run",G_CALLBACK(shika_filesystem_run),NULL);
  g_signal_connect(G_OBJECT(service),"run",G_CALLBACK(shika_service_run),NULL);
  g_socket_service_start(G_SOCKET_SERVICE(service));
  g_main_loop_run(main_loop);
}

static gboolean
shika_filesystem_run(GSocketService * service,
		  GSocketConnection * connection,
		  GObject * source,
		  gpointer data)
{
  g_print("FileSystem\n");
  return FALSE;
}

static gboolean
shika_service_run(GSocketService * service,
		   GSocketConnection * connection,
		   GObject * source,
		   gpointer data)
{
  g_autoptr(HttpRequest) request = http_request_new(HTTP_REQUEST_METHOD_GET,
						    "/",
						    1.1);
  g_autoptr(GError) error = NULL;

  GInputStream * input_stream =
      g_io_stream_get_input_stream(G_IO_STREAM(connection));

  g_autoptr(GDataInputStream) data_input =
      http_data_input_stream(input_stream, NULL, NULL, NULL);

  http_package_read_from_stream(HTTP_PACKAGE(request),
				data_input,
				NULL,
				NULL,
				NULL);

  ShikaBroadway * broadway = shika_broadway_new();
  shika_broadway_run(broadway,"/usr/bin/gnome-calculator",&error);
  if(error != NULL)
    g_print("%s",error->message);

  shika_broadway_redirect(broadway,request,G_IO_STREAM(connection),NULL);
  shika_broadway_kill(broadway);

  g_io_stream_close(G_IO_STREAM(connection),NULL,NULL);
  return TRUE;
}

