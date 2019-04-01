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

typedef struct
{
  GSocketService * service;
  ShikaContext ** context;
  HttpRequest * request;
  GSocketConnection * connection;
} ShikaRequestData;


gboolean
shika_request_idle(ShikaRequestData * data)
{
  g_autoptr(GError) error = NULL;
  ShikaHost * host = NULL;
  if(shika_context_is_set(*data->context,"viewport"))
    {
      host = (ShikaHost*)shika_context_get(*data->context,"viewport");
    }
  else
    {
      host = shika_host_new();
      shika_context_set(*data->context,"viewport",host,g_object_unref);
      shika_host_run(host,SHIKA_APPLICATION,"",&error);
    }

  if(shika_host_is_running(host))
    {
      shika_host_pipe(host,data->request,data->connection,&error);
    }
  else
    {
      shika_context_destroy(*data->context);
      g_autoptr(HttpResponse) response = http_response_new(HTTP_RESPONSE_OK,1.1);
	   GOutputStream * output = g_io_stream_get_output_stream(
	       G_IO_STREAM(data->connection));

      http_response_set_code(response,HTTP_RESPONSE_TEMPORARY_REDIRECT);
      http_package_set_string(HTTP_PACKAGE(response),
			      "Location","/login",-1);
      http_package_write_to_stream(
	  HTTP_PACKAGE(response),
	  output,
	  NULL,
	  NULL,
	  NULL);
      g_output_stream_flush(output,NULL,NULL);
    }

  g_object_unref(data->service);
  g_object_unref(data->request);
  g_object_unref(data->connection);
  g_free(data);
  return G_SOURCE_REMOVE;
}

gboolean
shika_application_layout(GSocketService * service,
			  ShikaContext ** context,
			  HttpRequest * request,
			  GSocketConnection * connection,
			  GError ** error)
{

  if(http_package_is_set(HTTP_PACKAGE(request),"Upgrade"))
    {
      const gchar * upgrade_to = http_package_get_string(HTTP_PACKAGE(request),
							 "Upgrade",NULL);
      if(g_strcmp0(upgrade_to,"websocket") == 0)
	{
	  ShikaRequestData * data = g_new0(ShikaRequestData,1);
	  data->service = g_object_ref(service);
	  data->request = g_object_ref(request);
	  data->connection = g_object_ref(connection);
	  data->context = context;
	  g_idle_add((GSourceFunc)shika_request_idle,data);
	  return FALSE;
	}
    }
  return TRUE;
}
