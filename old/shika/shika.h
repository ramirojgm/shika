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

#ifndef SHIKA_H_
#define SHIKA_H_

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>

#ifndef SHIKA_APPLICATION
  #define SHIKA_APPLICATION "mono ./bin/HiQuS.exe"
#endif

#ifndef SHIKA_FILESYSTEM_ROOT
  #define SHIKA_FILESYSTEM_ROOT (g_get_current_dir())
#endif


#include <http/httprequest.h>
#include <http/httpresponse.h>

#include <shikapipe.h>

#include <shikacontext.h>
#include <shikahost.h>
#include <shikabroadway.h>

typedef gboolean (*ShikaLayoutFunc)(GSocketService * service,
				    ShikaContext ** context,
				    HttpRequest * request,
				    GSocketConnection * connection,
				    GError ** error);


void		shika_register_layout(ShikaLayoutFunc * funct);

void		shika_service_run(guint16 port,guint32 max_tail);

#endif /* SHIKA_H_ */
