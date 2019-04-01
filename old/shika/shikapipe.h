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

#ifndef SHIKA_PIPE_H_
#define SHIKA_PIPE_H_

#define SHIKA_TYPE_PIPE		(shika_pipe_get_type())
G_DECLARE_FINAL_TYPE(ShikaPipe,shika_pipe,SHIKA,PIPE,GObject)

typedef struct _ShikaPipePrivate ShikaPipePrivate;

struct _ShikaPipeClass
{
  GObjectClass parent_class;
};

struct _ShikaPipe
{
  GObject parent_instance;

  ShikaPipePrivate * priv;
};

G_BEGIN_DECLS

GType		shika_pipe_get_type(void) G_GNUC_CONST;

ShikaPipe *	shika_pipe_new(void);

void		shika_pipe_open(ShikaPipe * pipe,
				GInputStream * input,
				GOutputStream * output);

gboolean	shika_pipe_is_open(ShikaPipe * pipe);

void		shika_pipe_close(ShikaPipe * pipe);

G_END_DECLS

#endif /* SHIKA_PIPE_H_ */
