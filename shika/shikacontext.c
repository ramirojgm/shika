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

static GMutex _shika_context_mutex = G_STATIC_MUTEX_INIT;
static GList * _shika_context_list = NULL;

typedef struct {
  gchar * name;
  gpointer data;
  GDestroyNotify free_func;
} ShikaContextAttribute;

ShikaContext *
shika_context_new(void)
{
  ShikaContext * context = g_new0(ShikaContext,1);
  g_mutex_lock(&_shika_context_mutex);
  for(;;)
    {
      context->context_id = g_uuid_string_random();
      for(GList * iter = g_list_first(_shika_context_list);
	  iter;
	  iter = g_list_next(iter))
	{
	  ShikaContext * sc = (ShikaContext*)iter->data;
	  g_mutex_lock(&sc->mutex);
	  if(g_strcmp0(sc->context_id,context->context_id) == 0)
	    {
	      g_free(context->context_id);
	      context->context_id = NULL;
	      g_mutex_unlock(&sc->mutex);
	      break;
	    }
	  else
	    {
	      g_mutex_unlock(&sc->mutex);
	    }
	}

      if(context->context_id)
	break;
    }
  context->timestamp = g_get_real_time();
  g_mutex_init(&context->mutex);
  context->attrs = NULL;
  _shika_context_list = g_list_append(_shika_context_list,context);
  g_mutex_unlock(&_shika_context_mutex);
  return context;
}

ShikaContext *
shika_context_get_by_id(const gchar * context_id)
{
  ShikaContext * context = NULL;
  g_mutex_lock(&_shika_context_mutex);
  for(GList * iter = g_list_first(_shika_context_list);
      iter;
      iter = g_list_next(iter))
    {
      ShikaContext * sc = (ShikaContext*)iter->data;
      g_mutex_lock(&sc->mutex);
      if(g_strcmp0(sc->context_id,context_id) == 0)
	  context = sc;
      g_mutex_unlock(&sc->mutex);
      if(context)
	break;
    }
  g_mutex_unlock(&_shika_context_mutex);
  return context;
}

const gchar *
shika_context_get_id(ShikaContext * context)
{
  gchar * context_id;
  g_mutex_lock(&context->mutex);
  context_id = context->context_id;
  g_mutex_unlock(&context->mutex);
  return context_id;
}

gboolean
shika_context_is_set(ShikaContext * context,
		     const gchar * attr_name)
{
  gboolean result = FALSE;
  g_mutex_lock(&context->mutex);
  for(GList * iter = g_list_first(context->attrs);
      iter;
      iter = g_list_next(iter))
    {
      ShikaContextAttribute * attr = (ShikaContextAttribute *) iter->data;
      if(g_strcmp0(attr->name,attr_name) == 0)
	{
	  result = TRUE;
	  break;
	}
    }
  g_mutex_unlock(&context->mutex);
  return result;
}

gconstpointer
shika_context_get(ShikaContext * context,
		  const gchar * attr_name)
{
  gconstpointer result = NULL;
  g_mutex_lock(&context->mutex);
  for(GList * iter = g_list_first(context->attrs);
        iter;
        iter = g_list_next(iter))
    {
      ShikaContextAttribute * attr = (ShikaContextAttribute *) iter->data;
      if(g_strcmp0(attr->name,attr_name) == 0)
      {
	result = attr->data;
	break;
      }
    }
  g_mutex_unlock(&context->mutex);
  return result;
}

void
shika_context_unset(ShikaContext * context,
		    const gchar * attr_name)
{
  g_mutex_lock(&context->mutex);
  for(GList * iter = g_list_first(context->attrs);
        iter;
        iter = g_list_next(iter))
    {
      ShikaContextAttribute * attr = (ShikaContextAttribute *) iter->data;
      if(g_strcmp0(attr->name,attr_name) == 0)
      {
	  context->attrs = g_list_remove_link(context->attrs,iter);
	  break;
      }
    }
  g_mutex_unlock(&context->mutex);
}

void
shika_context_set(ShikaContext * context,
		  const gchar * attr_name,
		  gconstpointer value,
		  GDestroyNotify free_func)
{
  gboolean exists = FALSE;
  g_mutex_lock(&context->mutex);
  for(GList * iter = g_list_first(context->attrs);
        iter;
        iter = g_list_next(iter))
    {
      ShikaContextAttribute * attr = (ShikaContextAttribute *) iter->data;
      if(g_strcmp0(attr->name,attr_name) == 0)
      {
	if(attr->data && attr->free_func)
	  attr->free_func(attr->data);
	attr->data = (gpointer)value;
	attr->free_func = free_func;
	exists = TRUE;
	break;
      }
    }

  if(!exists)
    {
      ShikaContextAttribute * attr = g_new0(ShikaContextAttribute,1);
      attr->name = g_strdup(attr_name);
      attr->data = (gpointer)value;
      attr->free_func = free_func;
      context->attrs = g_list_append(context->attrs,attr);
    }

  g_mutex_unlock(&context->mutex);
}

static void
shika_context_attribute_free(ShikaContextAttribute * attr)
{
  if(attr)
    {
      if(attr->data && attr->free_func)
	attr->free_func(attr->data);
      g_free(attr->name);
      g_free(attr);
    }
}

void
shika_context_destroy(ShikaContext * context)
{
  g_mutex_lock(&_shika_context_mutex);
  _shika_context_list = g_list_remove(_shika_context_list,context);
  g_mutex_lock(&context->mutex);
  g_free(context->context_id);
  g_list_free_full(context->attrs,(GDestroyNotify)shika_context_attribute_free);
  g_mutex_unlock(&context->mutex);
  g_mutex_clear(&context->mutex);
  g_free(context);
  g_mutex_unlock(&_shika_context_mutex);
}
