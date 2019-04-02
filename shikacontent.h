#ifndef SHIKA_CONTENT_H
#define SHIKA_CONTENT_H

typedef struct _ShikaContentPrivate ShikaContentPrivate;

#define SHIKA_TYPE_CONTENT  (shika_content_get_type())
G_DECLARE_FINAL_TYPE (ShikaContent,shika_content,SHIKA,CONTENT,GObject)

struct _ShikaContent
{
    GObject parent;
    
    ShikaContentPrivate * priv;
};

struct _ShikaContentClass
{
    GObjectClass parent_class;
    
};

G_BEGIN_DECLS

GType           shika_content_get_type(void);

ShikaContent *  shika_content_new();

void            shika_content_add_path (ShikaContent * sc, const gchar * path);

gboolean        shika_content_get (ShikaContent * sc,
                                    const gchar * path,
                                    const gchar ** mime_type,
                                    const gchar ** buffer, 
                                    gsize * length);

G_END_DECLS

#endif
