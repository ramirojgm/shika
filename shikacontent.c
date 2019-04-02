#include <shika.h>

typedef struct _ShikaFileContent ShikaFileContent;

struct _ShikaFileContent
{
    gchar * vfilename;
    gchar * mime_type;
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
shika_file_content_new(goffset voffset,const gchar * filename)
{
    ShikaFileContent * content = g_new0(ShikaFileContent,1);
    content->vfilename = g_strdup((gchar*)(voffset + filename));
    for (gchar * c = content->vfilename; *c ; c ++) 
    {
        if (*c == '\\') 
            *c = '/';
    }
    content->mime_type = g_strdup("text/html");
    g_file_get_contents(filename,&(content->content),&(content->content_length),NULL);
    return content;
}

static void
shika_file_content_free(ShikaFileContent * file)
{
    g_free(file->vfilename);
    g_free(file->mime_type);
    g_free(file->content);
    g_free(file->content_gzip);
    g_free(file->content_raw);
    g_free(file);
}

gboolean        
shika_content_get (ShikaContent * sc,
                    const gchar * path,
                    const gchar ** mime_type, 
                    const gchar ** buffer, 
                    gsize * length)
{
    g_autofree gchar * full_path = NULL;
    
    if (g_str_has_suffix(path,"/") || g_utf8_strlen(path,-1) == 0)
        full_path = g_build_path("/",path,"index.html",NULL);
    else
        full_path = g_build_path("/",path,NULL);

    ShikaFileContent * file_content = NULL;
    for (GList * iter = g_list_first(sc->priv->vfiles); iter; iter = g_list_next(iter))
    {
        file_content = (ShikaFileContent*) iter->data;
        if (g_strcmp0(file_content->vfilename,full_path) == 0)
        {
            *mime_type = file_content->mime_type;
            *buffer = file_content->content;
            *length = file_content->content_length;
            return TRUE;
        }
    }
    return FALSE;
}

void
_shika_content_add_path(ShikaContent * sc, goffset voffset, const gchar * path)
{
    GDir * dir = g_dir_open(path,0,NULL);
    const gchar * spath = NULL;
    while ((spath = g_dir_read_name(dir)))
    {
        g_autofree gchar * path_full = g_build_filename(path,spath,NULL); 
        if (g_file_test(path_full,G_FILE_TEST_IS_REGULAR))
            sc->priv->vfiles = g_list_append(sc->priv->vfiles,shika_file_content_new(voffset,path_full));
        else if (g_file_test(path_full,G_FILE_TEST_IS_DIR))
            _shika_content_add_path(sc,voffset, path_full);
    }
}

void            
shika_content_add_path (ShikaContent * sc, const gchar * path)
{
    _shika_content_add_path(sc,g_utf8_strlen(path,-1) + 1,path);
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
