#ifndef SHIKA_INSTANCE_H
#define SHIKA_INSTANCE_H

#define SHIKA_TYPE_INSTANCE     (shika_instance_get_type())
G_DECLARE_FINAL_TYPE(ShikaInstance,shika_instance,SHIKA,INSTANCE,GObject)

typedef struct _ShikaInstancePrivate ShikaInstancePrivate;
typedef struct _ShikaLaunch ShikaLaunch;

struct _ShikaInstanceClass
{
    GObjectClass parent_class;
};

struct _ShikaInstance
{
    GObject parent;
    ShikaInstancePrivate * priv;
};


struct _ShikaLaunch
{
    gchar ** env;
    gchar * wk_dir;
    gchar * ld_dir;
    gchar * binary;
    gchar * args;
};

G_BEGIN_DECLS

GType           shika_instance_get_type(void);

ShikaInstance * shika_instance_new();

gboolean        shika_instance_is_running (ShikaInstance * instance);

GPid            shika_instance_get_broadway_pid (ShikaInstance * instance);

GPid            shika_instance_get_application_pid (ShikaInstance * instance);

GIOStream *     shika_instance_get_stream (ShikaInstance * instance);

gboolean        shika_instance_run (ShikaInstance * instance,
                                    ShikaLaunch * launch, 
                                    GError ** error);

gboolean        shika_instance_stop (ShikaInstance * instance);


ShikaLaunch *       shika_launch_new (const gchar * binary,
                                       const gchar * args);

const gchar *      shika_launch_get_binary(ShikaLaunch * launch);

const gchar *      shika_launch_get_args (ShikaLaunch * launch);

void               shika_launch_add_env(ShikaLaunch * launch, 
                                            const gchar * name,
                                            const gchar * value);

const gchar **     shika_launch_get_env(ShikaLaunch * launch);

void               shika_launch_set_work_dir(ShikaLaunch * launch, 
                                                const gchar * dir);

const gchar *      shika_launch_get_work_dir(ShikaLaunch * launch);

void               shika_launch_set_ld_dir(ShikaLaunch * launch, 
                                                const gchar * dir);

const gchar *      shika_launch_get_ld_dir(ShikaLaunch * launch);

void               shika_launch_free (ShikaLaunch * launch);

G_END_DECLS

#endif
