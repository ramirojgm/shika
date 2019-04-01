#include <shika.h>

typedef struct _ShikaFileContent ShikaFileContent;

struct _ShikaFileContent
{
    gchar * vfilename;
    gchar * content;
    gsize   content_length;
    gchar * content_gzip;
    gsize   content_gzip_length;
    gchar * content_raw;
    gsize   content_raw_length;
};

struct _ShikaContentPrivate {
    GList *  vfiles;
    gpointer padding[12];
};

G_DEFINE_TYPE_WITH_PRIVATE(ShikaContent,shika_content,G_TYPE_OBJECT)


ShikaContent *
shika_content_new()
{
    return SHIKA_CONTENT(g_object_new(SHIKA_TYPE_CONTENT, NULL ));
}

static ShikaFileContent * 
shika_file_content_new(const gchar * vfilename)
{
    
 
}

static void
shika_file_content_free(ShikaFileContent * file)
{
    g_free(file->vfilename);
    g_free(file->content);
    g_free(file->content_gzip);
    g_free(file->content_raw);
    g_free(file);
}

void            
shika_content_add_path (ShikaContent * sc, const gchar * path)
{
    GDir * dir = g_dir_open(path,0,NULL);
    const gchar * spath = NULL;
    while ((spath = g_dir_read_name(dir)))
    {
        g_autofree gchar * path_full = g_build_filename(path,spath,NULL); 
        if (g_file_test(path_full,G_FILE_TEST_IS_REGULAR))
        {
            gchar * content = NULL;
            gsize content_length = 0;
            g_file_get_contents(path_full,&content,&content_length,NULL);
        }
        else if (g_file_test(path_full,G_FILE_TEST_IS_DIR))
            shika_content_add_path(sc,path_full);
    }
}

static void
shika_content_dispose(GObject * obj)
{
    ShikaContent * self = SHIKA_CONTENT(obj);
    g_list_free_full(self->priv->vfiles, (GDestroyNotify) shika_file_content_free);
}


static void
shika_content_init(ShikaContent * self)
{
    self->priv = shika_content_get_instance_private(self);
    self->priv->vfiles = NULL;
}


static void
shika_content_class_init(ShikaContentClass * klass)
{
    G_OBJECT_CLASS(klass)->dispose = shika_content_dispose;
}
