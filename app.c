#include <stdio.h>

#include "isogen_config.h"
#include "voxelgrid.h"
#include "settings.h"
#include "renderer.h"
#include "app.h"

static int main_cli (int argc, char **argv, Settings *settings)
{
    // load voxel data from file
    VoxelGrid *voxelgrid = voxelgrid_load ("in.vox");

    // initialize renderer
    Renderer *renderer = renderer_create();
    // render
    renderer_render(renderer, settings, voxelgrid);
    // write output file
    renderer_save(renderer, settings);
    // clean up render data
    renderer_free(renderer);

    voxelgrid_free(voxelgrid);


    // return success
    return 0;
}


static int main_gui (int argc, char **argv, Settings *settings)
{
    printf ("Gui not implemented yet\n");
    return 0;
}


int main(int argc, char **argv) {

    int opt_use_gui = 0;
    
    int opt;

    int result;

    // get user settings
    Settings *settings = settings_create();

    // Parse arguments
    while ( (opt = getopt(argc, argv, "gn")) != -1) {
        switch (opt) {
            case 'g':
                opt_use_gui = 1;
                break;
            case 'n':
                settings->numbered_faces = 1;
        }
    }

    if (opt_use_gui) {
        result = main_gui (argc, argv, settings);
    } else {
        result = main_cli (argc, argv, settings);
    }

    // free settings
    settings_free(settings);

    return result;
}
