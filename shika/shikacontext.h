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

#ifndef SHIKA_CONTEXT_H_
#define SHIKA_CONTEXT_H_

typedef struct _ShikaContext ShikaContext;

struct _ShikaContext
{
  GMutex mutex;

  gchar * context_id;
  gint64 timestamp;

  GList * attrs;
};

ShikaContext * 	shika_context_new(void);

ShikaContext *	shika_context_get_by_id(const gchar * context_id);

const gchar *	shika_context_get_id(ShikaContext * context);

gboolean	shika_context_is_set(ShikaContext * context,
				     const gchar * attr_name);

gconstpointer	shika_context_get(ShikaContext * context,
				  const gchar * attr_name);

void		shika_context_set(ShikaContext * context,
				  const gchar * attr_name,
				  gconstpointer value,
				  GDestroyNotify free_func);

void		shika_context_destroy(ShikaContext * context);

#endif /* SHIKA_CONTEXT_H_ */
