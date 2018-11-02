#include <gtk/gtk.h>
#include "interface.h"

int main(int argc,char *argv[]) {
    gtk_init (&argc, &argv);

    GtkWidget *window = generate_window();
    gtk_widget_show_all(window);

    gtk_main();
}
