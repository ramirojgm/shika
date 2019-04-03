#include <shika.h>

struct _ShikaApplicationPrivate {
    GMutex mutex;
    SoupServer * server;
    GList *  websockets;
    gpointer padding[12];
};

struct _ShikaApplicationContent
{
    ShikaApplication * app;
    goffset vpoffset;
    ShikaContent * content;
};

typedef struct _ShikaApplicationContent ShikaApplicationContent;

G_DEFINE_TYPE_WITH_PRIVATE(ShikaApplication,shika_application,G_TYPE_APPLICATION)

void
shika_application_activate(GApplication * app)
{
    ShikaApplication * self = SHIKA_APPLICATION(app);
    soup_server_listen_all (self->priv->server, 8080, 0, NULL);  
    GMainLoop * loop = g_main_loop_new(NULL,FALSE);
    g_main_loop_run(loop);
    g_main_loop_unref(loop); 
}

void 
shika_application_dispose (GObject * obj)
{
    ShikaApplication * app = SHIKA_APPLICATION(obj);
    g_clear_object(&app->priv->server);
}

GApplication *
shika_application_new()
{
    return G_APPLICATION(g_object_new(SHIKA_TYPE_APPLICATION,
        "application-id","org.gnome.shika.http",
        "flags",G_APPLICATION_FLAGS_NONE,
        NULL
    ));
}

static void
shika_application_content (SoupServer *server,
                            SoupMessage *msg,
                            const char *path,
                            GHashTable *query,
                            SoupClientContext *client,
                            gpointer data)
{
    ShikaApplicationContent * content = (ShikaApplicationContent*) data;
    const gchar * buffer = NULL;
    const gchar * mime_type = NULL;
    gsize length = 0;
    
    if (msg->method != SOUP_METHOD_GET) 
    {
		soup_message_set_status (msg, SOUP_STATUS_METHOD_NOT_ALLOWED);
		return;
	}
	
    if (shika_content_get(content->content,(content->vpoffset + path),&mime_type,&buffer,&length))
    {
        soup_message_set_status (msg, SOUP_STATUS_OK);
        soup_message_set_response (msg, mime_type, SOUP_MEMORY_COPY,buffer, length);
    }
    else
    {
        soup_message_set_status (msg, SOUP_STATUS_NOT_FOUND);
    }
}

static gboolean
shika_application_app_sock_destroy(gpointer data)
{
    g_object_unref(data);
    g_print("Destroyed\n");
    return G_SOURCE_REMOVE;
}

static void
shika_application_app_sock_close(SoupWebsocketConnection *connection,
                                 gpointer data)
{
    ShikaApplication * app = SHIKA_APPLICATION(data);
    app->priv->websockets = g_list_remove(app->priv->websockets,connection);    
    g_idle_add(shika_application_app_sock_destroy,connection);
}

static void
shika_application_app_sock_open (SoupServer *server,
                                SoupWebsocketConnection *connection,
                                const char *path,
                                SoupClientContext *client,
                                gpointer user_data)
{
    ShikaApplication * app = SHIKA_APPLICATION(user_data);
    //Push connection 
    app->priv->websockets = g_list_append(app->priv->websockets,g_object_ref(connection));
    g_signal_connect(connection,"closed",
        G_CALLBACK(shika_application_app_sock_close),
        app);

    g_print("path: %s\n",path);
    return;   
}

static ShikaApplicationContent * 
shika_application_content_new(ShikaApplication * app,goffset vpoffset,ShikaContent * content)
{
    ShikaApplicationContent * _app = g_new0(ShikaApplicationContent,1);
    _app->vpoffset = vpoffset;
    _app->app = g_object_ref(app);
    _app->content = g_object_ref(content);
    return _app;
}

static void 
shika_application_content_free(ShikaApplicationContent * app_content)
{
    g_object_unref(app_content->app);
    g_object_unref(app_content->content);
    g_free(app_content);
}

void
shika_application_add_content(ShikaApplication * self,
                              const gchar * vpath,
                              ShikaContent * content)
{
    soup_server_add_handler (self->priv->server,
                             vpath,
                             shika_application_content,
                             shika_application_content_new(self,g_utf8_strlen(vpath,-1),content),
                             (GDestroyNotify)shika_application_content_free);
}

gboolean
shika_application_auth_cb (SoupAuthDomain *domain,
                            SoupMessage *msg,
                            const char *username,
                            const char *password,
                            gpointer user_data)
{
    //TODO Use authentication
    return (g_strcmp0(username,"admin") == 0 && g_strcmp0(password,"Password1") == 0);
}

static void
shika_application_init(ShikaApplication * self)
{
    self->priv = shika_application_get_instance_private(self);
    self->priv->server = soup_server_new (SOUP_SERVER_SERVER_HEADER, "shika-httpd",
                                          NULL);
    self->priv->websockets = NULL;
    
    SoupAuthDomain * domain = soup_auth_domain_basic_new(
        SOUP_AUTH_DOMAIN_REALM,"Shika Application Streaming",
        SOUP_AUTH_DOMAIN_ADD_PATH,"/",
        NULL);
        
    soup_server_add_websocket_handler (self->priv->server,
                                        "/app.sock",
                                        NULL,
                                        NULL,
                                        shika_application_app_sock_open,
                                        self,
                                        NULL);
                                        
    soup_auth_domain_basic_set_auth_callback (domain,shika_application_auth_cb,self,NULL);
    soup_server_add_auth_domain(self->priv->server, domain);
    g_object_unref (domain);
}


static void
shika_application_class_init(ShikaApplicationClass * klass)
{
    G_APPLICATION_CLASS(klass)->activate = shika_application_activate;
}

