#ifndef SHIKA_APPLICATION_H
#define SHIKA_APPLICATION_H

#define SHIKA_TYPE_APPLICATION (shika_application_get_type())
G_DECLARE_FINAL_TYPE (ShikaApplication,shika_application,SHIKA,APPLICATION,GApplication)

typedef struct _ShikaApplicationPrivate ShikaApplicationPrivate;

struct _ShikaApplication {
    GApplication parent;
    
    ShikaApplicationPrivate * priv;
};

struct _ShikaApplicationClass {
    GApplicationClass parent_class;
};

G_BEGIN_DECLS

GType           shika_application_get_type(void);

GApplication *  shika_application_new();

void            shika_application_add_content(ShikaApplication * app,
                                              const gchar * vpath,
                                              ShikaContent * content);

G_END_DECLS

#endif
