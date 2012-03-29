
#include <gtk/gtk.h>

#include "settings.h"

static void gui_setup_main_window (Settings *settings)
{
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *image;
    GtkWidget *label;

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_signal_connect (G_OBJECT (window), "destroy", gtk_main_quit, NULL);

    vbox = gtk_vbox_new (FALSE, 2);
    gtk_container_add (GTK_CONTAINER (window), vbox);

    image = gtk_image_new_from_file ("out.png");
    gtk_box_pack_start (GTK_BOX (vbox), image, TRUE, TRUE, 2);

    label = gtk_label_new ("Hello, IsoGen!");
    gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, TRUE, 2);

    gtk_widget_show_all (window);
}

int gui_main (int argc, char **argv, Settings *settings)
{
    log_printf(2, "Entering GUI-mode\n");

    gtk_init (&argc, &argv);

    gui_setup_main_window (settings);

    gtk_main ();

    log_printf(2, "Exiting GUI-mode\n");
    return 0;
}
