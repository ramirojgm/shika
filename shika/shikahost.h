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

#ifndef SHIKA_HOST_H_
#define SHIKA_HOST_H_

#define SHIKA_TYPE_HOST	(shika_host_get_type())
G_DECLARE_FINAL_TYPE(ShikaHost,shika_host,SHIKA,HOST,GObject)

typedef struct _ShikaHostPrivate ShikaHostPrivate;

struct _ShikaHost
{
  GObject parent_instance;
  ShikaHostPrivate * priv;
};

struct _ShikaHostClass
{
  GObjectClass parent_class;
};

G_BEGIN_DECLS

GType		shika_host_get_type(void) G_GNUC_CONST;

ShikaHost *	shika_host_new();

const GSocketAddress *
		shika_host_get_address(ShikaHost * host);

guint		shika_host_get_display(ShikaHost * host);

gboolean	shika_host_is_connected(ShikaHost * host);

gboolean	shika_host_is_running(ShikaHost * host);

gboolean	shika_host_pipe(ShikaHost * host,
				HttpRequest * request,
				GSocketConnection * remote,
				GError ** error);

gboolean	shika_host_run(ShikaHost * host,
			       const gchar * program_path,
			       const gchar * arguments,
			       GError ** error);

void		shika_host_kill(ShikaHost * host);

G_END_DECLS

#endif /* SHIKA_DISPLAY_H_ */
