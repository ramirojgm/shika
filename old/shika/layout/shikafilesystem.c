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
shika_filesystem_layout(GSocketService * service,
		      ShikaContext ** context,
		      HttpRequest * request,
		      GSocketConnection * connection,
		      GError ** error)
{
  if(http_request_get_method(request) == HTTP_REQUEST_METHOD_GET)
    {
      g_autoptr(HttpResponse) response = http_response_new(HTTP_RESPONSE_OK,
							   1.1);
      GOutputStream * output =
	  g_io_stream_get_output_stream(G_IO_STREAM(connection));

      const gchar * query_filename = http_request_get_query(request);

      if(g_path_is_absolute(query_filename))
	{

	  g_autofree gchar * full_path = g_build_filename(
	      SHIKA_FILESYSTEM_ROOT,
	      "html",
	      query_filename,
	      NULL);

	  if (g_file_test(full_path,G_FILE_TEST_IS_DIR))
	    {
	      g_free(full_path);
	      full_path = g_build_filename(
			  g_get_current_dir(),
			  "html",
			  query_filename,
			  "index.html",
			  NULL);
	    }

	  if(g_file_test(full_path,G_FILE_TEST_EXISTS))
	    {
	      g_autofree gchar * content = NULL;
	      gsize content_size;
	      g_autofree gchar * mimetype = NULL;

	      g_file_get_contents(full_path,&content,&content_size,NULL);
	      mimetype = g_content_type_guess (full_path,
				               (guchar*)content,
				               content_size,
				               NULL);
	      http_package_set_string(HTTP_PACKAGE(response),
				      "Content-Type",
				      mimetype,-1);

	      http_package_set_int64(HTTP_PACKAGE(response),
				     "Content-Length",content_size);

	      http_package_write_to_stream(HTTP_PACKAGE(response),output,
					   NULL,NULL,NULL);
	      g_output_stream_flush(output,NULL,NULL);
	      g_output_stream_write_all(output,content,content_size,
					NULL,NULL,NULL);
	      g_output_stream_write_all(output,"\r\n\r\n",4,
					NULL,NULL,NULL);

	    }
	  else
	    {
	      http_response_set_code(response,HTTP_RESPONSE_NOT_FOUND);
	      http_package_write_to_stream(HTTP_PACKAGE(response),output,
	      					   NULL,NULL,NULL);
	      g_output_stream_flush(output,NULL,NULL);
	      g_output_stream_write_all(output,"\r\n\r\n",4,
					NULL,NULL,NULL);
	    }

	}
      else
	{
	  //Forbidden
	  g_print("POST\n");
	}
    }
  else
    {

    }

  return FALSE;
}
