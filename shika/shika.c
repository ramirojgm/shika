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
#include <layout/shikasecurity.h>
#include <layout/shikafilesystem.h>
#include <layout/shikaapplication.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>


static gboolean
_shika_request_run(GSocketService * service,
		   GSocketConnection * connection,
		   GObject * source,
		   gpointer data)
{
  g_autoptr(HttpRequest) request = http_request_new(HTTP_REQUEST_METHOD_GET,
						    "/",
						    1.1);
  g_autoptr(GError) error = NULL;

  int flag = 1;

  GSocket * socket = NULL;
  socket = g_socket_connection_get_socket (connection);
  setsockopt (g_socket_get_fd (socket), IPPROTO_TCP,
              TCP_NODELAY, (char *) &flag, sizeof(int));

  GInputStream * input_stream =
      g_io_stream_get_input_stream(G_IO_STREAM(connection));

  g_autoptr(GDataInputStream) data_input =
      http_data_input_stream(input_stream, NULL, NULL, NULL);

  http_package_read_from_stream(HTTP_PACKAGE(request),
				data_input,
				NULL,
				NULL,
				NULL);

  ShikaContext * context = NULL;

  if (shika_security_layout(service,&context,request,connection,&error))
    {
      if(shika_application_layout(service,&context,request,connection,&error))
	{
	  if (shika_filesystem_layout(service,&context,request,connection,&error))
	    {
	      //TODO: Others layout
	    }
	}
    }

  if(error)
    {
      //TODO: Process error
    }

  return TRUE;
}

void
shika_service_run(guint16 port,guint32 max_tail)
{
  g_autoptr(GMainLoop) main_loop = g_main_loop_new(NULL,FALSE);
  g_autoptr(GSocketService) service = g_threaded_socket_service_new(max_tail);

  g_socket_listener_add_inet_port(G_SOCKET_LISTENER(service),
				port,
				NULL,
				NULL);

  g_signal_connect(G_OBJECT(service),"run",G_CALLBACK(_shika_request_run),NULL);
  g_socket_service_start(G_SOCKET_SERVICE(service));
  g_main_loop_run(main_loop);
}



