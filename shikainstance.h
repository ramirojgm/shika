#ifndef SHIKA_INSTANCE_H
#define SHIKA_INSTANCE_H

#define SHIKA_TYPE_INSTANCE     (shika_instance_get_type())
G_DECLARE_FINAL_TYPE(ShikaInstance,shika_instance,SHIKA,INSTANCE,GObject)

typedef struct _ShikaInstancePrivate ShikaInstancePrivate;

struct _ShikaInstanceClass
{
    GObjectClass parent_class;
};

struct _ShikaInstance
{
    GObject parent;
    ShikaInstancePrivate * priv;
};


G_BEGIN_DECLS

GType           shika_instance_get_type(void);

ShikaInstance * shika_instance_new();

gboolean        shika_instance_is_running (ShikaInstance * instance);

GPid            shika_instance_get_broadway_pid (ShikaInstance * instance);

GPid            shika_instance_get_application_pid (ShikaInstance * instance);

GIOStream *     shika_instance_get_stream (ShikaInstance * instance);

gboolean        shika_instance_run (ShikaInstance * instance,const gchar * launcher, GError ** error);

gboolean        shika_instance_stop (ShikaInstance * instance);

G_END_DECLS

#endif