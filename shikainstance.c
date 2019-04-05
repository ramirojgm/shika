#include <shika.h>

struct _ShikaInstancePrivate {
    GPid application_pid;
    GPid broadway_pid;
        
    GIOStream * stream;
};


G_DEFINE_TYPE_WITH_PRIVATE (ShikaInstance,shika_instance,G_TYPE_OBJECT)

ShikaInstance * 
shika_instance_new()
{
    return SHIKA_INSTANCE(g_object_new(SHIKA_TYPE_INSTANCE,NULL));
}

gboolean        
shika_instance_is_running (ShikaInstance * instance)
{
    return instance->priv->application_pid != 0;
}

GPid            
shika_instance_get_broadway_pid (ShikaInstance * instance)
{
    return instance->priv->broadway_pid;
}

GPid            
shika_instance_get_application_pid (ShikaInstance * instance)
{
    return instance->priv->application_pid;
}

GIOStream *     
shika_instance_get_stream (ShikaInstance * instance)
{
    return instance->priv->stream;
}

gboolean        
shika_instance_run (ShikaInstance * instance,
                    ShikaLaunch * launch, 
                    GError ** error)
{}

gboolean        
shika_instance_stop (ShikaInstance * instance)
{}

static void
shika_instance_init(ShikaInstance * self)
{
    self->priv = shika_instance_get_instance_private(self);
    self->priv->application_pid = 0;
    self->priv->broadway_pid = 0;
    self->priv->stream = NULL;
}

static void
shika_instance_class_init(ShikaInstanceClass * klass)
{

}


ShikaLaunch *       
shika_launch_new (const gchar * binary,
                   const gchar * args)
{
    ShikaLaunch * launch = g_new0(ShikaLaunch,1);
    launch->binary = g_strdup(binary);
    launch->args = g_strdup(args);
    launch->env = g_array_new (TRUE,FALSE,sizeof(gchar*));
    return launch;
}

const gchar *      
shika_launch_get_binary(ShikaLaunch * launch)
{
    return launch->binary;
}

const gchar *      
shika_launch_get_args (ShikaLaunch * launch)
{
    return launch->args;
}

void               
shika_launch_add_env(ShikaLaunch * launch, 
                        const gchar * name,
                        const gchar * value)
{
    gchar * val = g_strdup_printf("%s=%s",name,value);
    g_array_append_val (launch->env,val);
}

const gchar **     
shika_launch_get_env(ShikaLaunch * launch)
{
    return (const gchar**) (launch->env->data);
}

void               
shika_launch_set_work_dir(ShikaLaunch * launch, 
                            const gchar * dir)
{
    g_clear_pointer(&launch->wk_dir,g_free);
    launch->wk_dir = g_strdup(dir);
}

const gchar *      
shika_launch_get_work_dir(ShikaLaunch * launch)
{
    return launch->wk_dir;
}

void               
shika_launch_set_ld_dir(ShikaLaunch * launch, 
                            const gchar * dir)
{
    g_clear_pointer(&launch->ld_dir,g_free);
    launch->ld_dir = g_strdup(dir);
}

const gchar *      
shika_launch_get_ld_dir(ShikaLaunch * launch)
{
    return launch->ld_dir;
}

void               
shika_launch_free (ShikaLaunch * launch)
{
    g_strfreev((gchar **)launch->env->data);
    g_array_free(launch->env,FALSE);
    g_free(launch->binary);
    g_free(launch->args);
    g_free(launch->ld_dir);
    g_free(launch->wk_dir);
    g_free(launch);
}

