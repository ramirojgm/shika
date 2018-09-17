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
shika_security_layout(GSocketService * service,
		      ShikaContext ** context,
		      HttpRequest * request,
		      GSocketConnection * connection,
		      GError ** error)
{
  if(http_package_is_set(HTTP_PACKAGE(request),"Cookie"))
    {
      const gchar * strcookies = http_package_get_string(HTTP_PACKAGE(request),
							 "Cookie",
							 NULL);
      gchar ** cookies = g_strsplit(strcookies,";",10);
      gint cookies_len = g_strv_length(cookies);
      for(gint cookie_index = 0; cookie_index < cookies_len; cookie_index ++)
	{
	  gchar ** cookie = g_strsplit(cookies[cookie_index],"=",2);
	  gint cookie_len = g_strv_length(cookie);
	  if(cookie_len == 2)
	    {
	      g_strstrip(cookie[0]);
	      g_strstrip(cookie[1]);

	      if(g_strcmp0(cookie[0],"uuid") == 0)
		  *context = shika_context_get_by_id(cookie[1]);
	    }
	  g_strfreev(cookie);
	}
      g_strfreev(cookies);
    }

  if (*context == NULL)
    {
      g_autoptr(HttpResponse) response = http_response_new(HTTP_RESPONSE_OK,1.1);
      GOutputStream * output = g_io_stream_get_output_stream(G_IO_STREAM(connection));

      const gchar * query = http_request_get_query(request);
      HttpRequestMethod method = http_request_get_method(request);

      gboolean is_public =  g_str_has_prefix(query,"/login") ||
			    g_str_has_prefix(query,"/css") ||
			    g_str_has_prefix(query,"/img") ||
			    g_str_has_prefix(query,"/js") ||
			    g_str_has_prefix(query,"/favicon.ico");

      if (g_strcmp0(query,"/login/sign_in") == 0)
	{
	  *context = shika_context_new();

	  const gchar * redirect = "<html><head><meta http-equiv=\"refresh\" content=\"5; URL=/app\" /></head></html>";
	  gsize redirect_size = g_utf8_strlen(redirect,G_MAXINT);

	  g_autofree gchar * setcookie = g_strdup_printf(
	      "uuid=%s; httponly; Path=/",shika_context_get_id(*context));

	  http_response_set_code(response,HTTP_RESPONSE_OK);
	  http_package_set_string(HTTP_PACKAGE(response),
				  "Refresh","0; url=/app",-1);

	  http_package_set_string(HTTP_PACKAGE(response),
				  "Set-Cookie",setcookie,-1);

	  http_package_set_string(HTTP_PACKAGE(response),
				  "Content-Type","text/html",-1);

	  http_package_set_int64(HTTP_PACKAGE(response),
				  "Content-Length",redirect_size);

	  http_package_write_to_stream(
	      HTTP_PACKAGE(response),
	      output,
	      NULL,
	      NULL,
	      NULL);

	  g_output_stream_write_all(output,redirect,redirect_size,
				    NULL,NULL,NULL);

	  g_output_stream_flush(output,NULL,NULL);
	  g_usleep(1000);
	  return FALSE;
	}
      else if (!(is_public && method == HTTP_REQUEST_METHOD_GET))
	{
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
	  g_usleep(1000);
	  return FALSE;
	}
    }

  return TRUE;
}
