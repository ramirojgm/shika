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

#ifndef SHIKA_BROADWAY_H_
#define SHIKA_BROADWAY_H_

#define SHIKA_TYPE_BROADWAY	(shika_broadway_get_type())
G_DECLARE_FINAL_TYPE(ShikaBroadway,shika_broadway,SHIKA,BROADWAY,GObject)

struct _ShikaBroadway
{
  GObject parent_instance;
  GMutex mutex;
};

struct _ShikaBroadwayClass
{
  GObjectClass parent_class;
};

G_BEGIN_DECLS

GType			shika_broadway_get_type(void) G_GNUC_CONST;

ShikaBroadway * 	shika_broadway_new();

gboolean		shika_broadway_is_running(ShikaBroadway * broadway);

gboolean		shika_broadway_run(ShikaBroadway * broadway,
					   const gchar * program_path,
					   GError ** error);

gboolean		shika_broadway_redirect(ShikaBroadway * broadway,
						HttpRequest * request,
						GIOStream * stream,
						GError ** error);

void			shika_broadway_kill(ShikaBroadway * broadway);

G_END_DECLS

#endif /* SHIKA_BROADWAY_H_ */
