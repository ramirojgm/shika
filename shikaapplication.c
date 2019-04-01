#include <shika.h>


struct _ShikaApplicationPrivate {
    GMutex mutex;
    SoupServer * server;
    gpointer padding[12];
};

struct _ShikaApplicationContent
{
    ShikaApplication * app;
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
        "application-id","org.gnome.shika",
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
                            gpointer user_data)
{
    g_print("File [ %s ]\n",path);
}

static ShikaApplicationContent * 
shika_application_content_new(ShikaApplication * app,ShikaContent * content)
{
    ShikaApplicationContent * _app = g_new0(ShikaApplicationContent,1);
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
                             shika_application_content_new(self,content),
                             (GDestroyNotify)shika_application_content_free);
}

static void
shika_application_init(ShikaApplication * self)
{
    self->priv = shika_application_get_instance_private(self);
    self->priv->server = soup_server_new (SOUP_SERVER_SERVER_HEADER, "shika-httpd",
                                          NULL);
}


static void
shika_application_class_init(ShikaApplicationClass * klass)
{
    G_APPLICATION_CLASS(klass)->activate = shika_application_activate;
}

