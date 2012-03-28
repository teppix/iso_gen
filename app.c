#include "isogen_config.h"
#include "voxelgrid.h"
#include "settings.h"
#include "renderer.h"
#include "app.h"

static int main_cli (int argc, char **argv)
{
    // load voxel data from file
    VoxelGrid *voxelgrid = voxelgrid_load ("in.vox");

    // get user settings
    Settings *settings = settings_create();

    // initialize renderer
    Renderer *renderer = renderer_create();
    // render
    renderer_render(renderer, settings, voxelgrid);
    // write output file
    renderer_save(renderer, settings);
    // clean up render data
    renderer_free(renderer);

    voxelgrid_free(voxelgrid);

    settings_free(settings);

    // return success
    return 0;
}


static int main_gui (int argc, char **argv)
{
    printf ("Gui not implemented yet\n");
    return 0;
}


int main(int argc, char **argv) {

    int opt_use_gui = 0;

    int opt;

    // Parse arguments
    while ( (opt = getopt(argc, argv, "g")) != -1) {
        switch (opt) {
            case 'g':
                opt_use_gui = 1;
                break;
        }
    }

    if (opt_use_gui) {
        return main_gui (argc, argv);
    } else {
        return main_cli (argc, argv);
    }
}
