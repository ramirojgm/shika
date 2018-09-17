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
	  ShikaBroadway * viewport = NULL;
	  if(shika_context_is_set(*context,"viewport"))
	    {
	      viewport = (ShikaBroadway*)shika_context_get(*context,"viewport");
	    }
	  else
	    {
	      viewport = shika_broadway_new();
	      shika_context_set(*context,"viewport",viewport,g_object_unref);
	      shika_broadway_run(viewport,SHIKA_APPLICATION,"",error);
	    }

	  if(shika_broadway_is_running(viewport))
	    {
	      shika_broadway_redirect(viewport,
				      request,
				      G_IO_STREAM(connection),
				      error);

	      if(!shika_broadway_is_running(viewport))
		shika_context_destroy(*context);
	    }
	  else
	    {
	      shika_context_destroy(*context);
	      g_autoptr(HttpResponse) response = http_response_new(HTTP_RESPONSE_OK,1.1);
	           GOutputStream * output = g_io_stream_get_output_stream(G_IO_STREAM(connection));

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
	      return FALSE;
	    }
	  return FALSE;
	}
    }
  return TRUE;
}
