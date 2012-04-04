
#include <gtk/gtk.h>
#include <stdlib.h>

#include "sphere_generator.h"
#include "generator.h"
#include "settings.h"
#include "renderer.h"


typedef struct GuiContext GuiContext;
struct GuiContext
{
    GdkPixmap *pixmap;
    Settings *settings;
    Renderer *renderer;
    Generator *generator;

    GtkWidget *iso_view;
};

static void iso_view_expose (GtkWidget *widget, GdkEventExpose *event, GuiContext *context)
{
    char buffer[128];
    Renderer *renderer = context->renderer;

    cairo_t *cr;

    if (renderer->pixels == NULL) {
        printf ("nothing\n");
        return;
    }

    // Create cairo surface from pixel data
    /*
    source = cairo_image_surface_create_for_data (
            (char*)renderer->pixels,
            CAIRO_FORMAT_RGB24,
            renderer->image_width,
            renderer->image_height,
            renderer->image_width * 4
    );
    */

    cr = gdk_cairo_create (widget->window);
    
    cairo_set_source_rgb (cr, .5, .5, .5);
    cairo_paint (cr);

    // copy image from source-surface
    cairo_set_source_surface (cr, renderer->surface, 0, 0);
    cairo_paint (cr);

    cairo_destroy (cr);
}

static void adjustment_changed_cb (GtkAdjustment *adj, GuiContext *context)
{
    int value = (int)gtk_adjustment_get_value(adj);
    VoxelGrid *voxelgrid;
    Generator *gen = context->settings->generator;
    SphereGenerator *sgen = (SphereGenerator*)gen->generator_instance;

    if (value == sgen->radius) {
        // nothing new...
        return;
    }
    sgen->radius = value;

    voxelgrid = generator_generate (gen);

    renderer_render (context->renderer, context->settings, voxelgrid);

    voxelgrid_free (voxelgrid);
    gtk_widget_queue_draw (context->iso_view);

}

static void gui_setup_main_window (GuiContext *context)
{
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *iso_view;
    GtkObject *sphere_adj;
    GtkWidget *slider;

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_signal_connect (G_OBJECT (window), "destroy", gtk_main_quit, NULL);

    vbox = gtk_vbox_new (FALSE, 2);
    gtk_container_add (GTK_CONTAINER (window), vbox);

    context->iso_view = iso_view = gtk_drawing_area_new ();
    gtk_box_pack_start (GTK_BOX (vbox), iso_view, TRUE, TRUE, 2);

    sphere_adj = gtk_adjustment_new (
            ((SphereGenerator*)context->settings->generator->generator_instance)->radius,
            1, 10, // min, max
            1, // step inc
            1, // page inc
            1 // page size
    );

    slider = gtk_hscale_new (GTK_ADJUSTMENT (sphere_adj));
    gtk_scale_set_digits (GTK_SCALE (slider), 0);
    gtk_box_pack_start (GTK_BOX (vbox), slider, FALSE, TRUE, 2);

    gtk_widget_show_all (window);

    g_signal_connect (G_OBJECT (iso_view), "expose-event",
            G_CALLBACK (iso_view_expose), (gpointer)context);
    g_signal_connect (sphere_adj, "value-changed",
            G_CALLBACK (adjustment_changed_cb), (gpointer)context);
}

int gui_main (int argc, char **argv, Settings *settings)
{
    GuiContext context;
    VoxelGrid *voxelgrid;
    SphereGenerator *gen_sphere;

    log_printf(2, "Entering GUI-mode\n");

    //context = malloc (sizeof (GuiContext));
    context.settings = settings;
    context.pixmap = NULL;
    context.renderer = renderer_create();

    if (settings->generator == NULL) {
        log_printf (0, "Generator needed for gui mode\n");
        return 1;
    }

    if (settings->generator->type != GENERATOR_SPHERE) {
        log_printf (0, "Generator not supported in gui mode yet\n");
        return 1;
    }

    gen_sphere = (SphereGenerator*)settings->generator->generator_instance;

    voxelgrid = generator_generate(settings->generator);
    renderer_render(context.renderer, settings, voxelgrid);
    voxelgrid_free (voxelgrid);

    gtk_init (&argc, &argv);

    gui_setup_main_window (&context);

    gtk_main ();

    renderer_free (context.renderer);

    log_printf(2, "Exiting GUI-mode\n");
    return 0;
}
