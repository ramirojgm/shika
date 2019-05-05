#include <shika.h>
#include <sys/stat.h>
#include <gio-unix-2.0/gio/gunixsocketaddress.h>

struct _ShikaInstancePrivate {
    GPid application_pid;
    GPid broadway_pid;
        
    SoupWebsocketConnection * websocket;
    GSocketConnection * stream;
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

SoupWebsocketConnection *
shika_instance_get_websocket (ShikaInstance * instance)
{
    return instance->priv->websocket;
}

gboolean        
shika_instance_run (ShikaInstance * instance,
                    ShikaLaunch * launch, 
                    GError ** error)
{
  static volatile gchar * shika_temp_dir = NULL;
  static volatile guint32 shika_instance_id = 0;
  shika_instance_id ++;

  gboolean done = FALSE;

  if (shika_temp_dir == NULL)
    {
      shika_temp_dir = g_strdup_printf("/tmp/shika.%ux",
				       (guint32)(&shika_instance_id));
      mkdir((const gchar*)shika_temp_dir,S_IRWXU);
    }

  g_autofree gchar * shika_unix_path =
      g_strdup_printf("shika_%x_child%u.socket",&shika_instance_id,
		      shika_instance_id);

  g_autofree gchar * shika_display =
      g_strdup_printf(":%u",shika_instance_id);

  const gchar * broadwayd_argv[] = {
      "/usr/bin/broadwayd",
      shika_display,
      "-u",
      shika_unix_path,
      NULL};

  if (g_spawn_async(NULL,
		    (gchar**)broadwayd_argv,
		    NULL,
		    G_SPAWN_DEFAULT,
		    NULL,
		    NULL,
		    &instance->priv->broadway_pid,
		    error))
    {
      gchar **	app_argv = g_strsplit(launch->args," ",48);
      guint	app_argc = g_strv_length(app_argv);

      gchar ** 	app_full_argv = g_new0(gchar*,app_argc + 2);

      app_full_argv[0] = g_strdup(launch->binary);

      for (guint argv_index = 0; argv_index < app_argc; argv_index ++)
	app_full_argv[argv_index + 1] = g_strdup(app_argv[argv_index]);

      gchar ** env_envp = g_get_environ();
      env_envp = g_environ_setenv(env_envp,
				  "BROADWAY_DISPLAY",shika_display,
				  TRUE);

      env_envp = g_environ_setenv(env_envp,
      				  "GDK_BACKEND","broadway",
				  TRUE);
      g_strfreev(app_argv);

      if (g_spawn_async(launch->wk_dir,
			app_full_argv,
			env_envp,
			G_SPAWN_DO_NOT_REAP_CHILD|G_SPAWN_FILE_AND_ARGV_ZERO,
			NULL,
			NULL,
			&instance->priv->application_pid,
			error))
	{
	  GSocketAddress * address = g_unix_socket_address_new (shika_unix_path);

	  GSocket * socket = g_socket_new(G_SOCKET_FAMILY_UNIX,
					  G_SOCKET_TYPE_STREAM,
					  G_SOCKET_PROTOCOL_DEFAULT,
					  error);

	  if (g_socket_connect(socket,address,NULL,error))
	    {
	      instance->priv->stream =
		  g_socket_connection_factory_create_connection(socket);
	    }
	  else
	    {
	      g_print("Kill:%s- %s\n",shika_unix_path, (*error)->message);
	      //kill(instance->priv->broadway_pid,SIGKILL);
	      //kill(instance->priv->application_pid,SIGKILL);
	    }
	}

      g_strfreev(env_envp);
      g_strfreev(app_full_argv);
    }
  return done;
}

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

