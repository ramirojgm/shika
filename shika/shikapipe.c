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

struct _ShikaPipePrivate
{
  GInputStream * input;
  GOutputStream * output;
  GSource * source;
};

enum ShikaPipeSignals
{
  SHIKA_PIPE_SIGNAL_CLOSE,
  SHIKA_PIPE_N_SIGNALS
};

static guint shika_pipe_signals[SHIKA_PIPE_N_SIGNALS] = {0,};

G_DEFINE_TYPE_WITH_PRIVATE(ShikaPipe,shika_pipe,G_TYPE_OBJECT)

static void
shika_pipe_init(ShikaPipe * self)
{
  self->priv = shika_pipe_get_instance_private(self);
  self->priv->input = NULL;
  self->priv->output = NULL;
  self->priv->source = NULL;
}

static void
shika_pipe_dispose(GObject * object)
{
  ShikaPipe * self = SHIKA_PIPE(object);
  if(shika_pipe_is_open(self))
    shika_pipe_close(self);
}

static gboolean
shika_pipe_close_idle(gpointer data)
{
  shika_pipe_close(SHIKA_PIPE(data));
  g_signal_emit(data,
		shika_pipe_signals[SHIKA_PIPE_SIGNAL_CLOSE],
		0);
  return G_SOURCE_REMOVE;
}

static gboolean
shika_pipe_callback (GObject *pollable_stream,
                     gpointer data)
{
  ShikaPipe * pipe = SHIKA_PIPE(data);
  gchar buffer[1024] = {0,};

  gsize read = g_pollable_input_stream_read_nonblocking (
      G_POLLABLE_INPUT_STREAM(pipe->priv->input),
      buffer,
      sizeof(buffer),
      NULL,
      NULL
  );

  if(read > 0)
      g_output_stream_write_all(pipe->priv->output,buffer,read,NULL,NULL,NULL);

  if(g_input_stream_is_closed(pipe->priv->input) ||
     g_output_stream_is_closed(pipe->priv->output))
    {
      g_idle_add(shika_pipe_close_idle,pipe);
      return G_SOURCE_REMOVE;
    }
  else
    {
      return G_SOURCE_CONTINUE;
    }
}

ShikaPipe *
shika_pipe_new(void)
{
  return SHIKA_PIPE(g_object_new(SHIKA_TYPE_PIPE,NULL));
}

void
shika_pipe_open(ShikaPipe * pipe,
		GInputStream * input,
		GOutputStream * output)
{
  pipe->priv->source = g_pollable_input_stream_create_source(
      G_POLLABLE_INPUT_STREAM(input),NULL);

  pipe->priv->input = input;
  pipe->priv->output = output;

  g_source_set_callback(pipe->priv->source,
			(GSourceFunc)shika_pipe_callback,
			pipe,
			NULL);
}

static void
shika_pipe_class_init(ShikaPipeClass * klass)
{
  shika_pipe_signals[SHIKA_PIPE_SIGNAL_CLOSE] =
      g_signal_new("closed",
		   G_TYPE_FROM_CLASS(klass),
		   G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
		   0,
		   NULL,
		   NULL,
		   NULL,
		   G_TYPE_NONE,
		   0,
		   NULL);

  G_OBJECT_CLASS(klass)->dispose = shika_pipe_dispose;
}

gboolean
shika_pipe_is_open(ShikaPipe * pipe)
{
  return (pipe->priv->input && pipe->priv->output && pipe->priv->source);
}

void
shika_pipe_close(ShikaPipe * pipe)
{
  if(pipe->priv->input)
    {
      if(!g_input_stream_is_closed(pipe->priv->input))
	g_input_stream_close(pipe->priv->input,NULL,NULL);
      pipe->priv->input = NULL;
    }

  if(pipe->priv->output)
    {
      if(!g_output_stream_is_closed(pipe->priv->output))
	g_output_stream_close(pipe->priv->output,NULL,NULL);
      pipe->priv->output = NULL;
    }

  g_clear_pointer(&(pipe->priv->source),g_source_destroy);
}
