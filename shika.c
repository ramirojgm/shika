#include <shika.h>

gint main(gint argc,gchar ** argv)
{
    GApplication * app = shika_application_new();
    g_autoptr(ShikaContent) content = shika_content_new();
    shika_content_add_path(content,"./html");
    shika_application_add_content(SHIKA_APPLICATION(app),"/",content);
    return g_application_run(app,argc,argv);
}
