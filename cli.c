
#include <string.h>
#include <stdio.h>

#include "cli.h"
#include "voxelgrid.h"
#include "renderer.h"

#include "app.h"

int cli_main (int argc, char **argv, Settings *settings)
{
    log_printf(2, "Entering CLI-mode\n");
    // check input filename
    if (strlen(settings->in_filename) == 0) {
        fprintf (stderr, "Error: No input file specified\n");
        suggest_help();
        return 1;
    }

    // load voxel data from file
    VoxelGrid *voxelgrid = voxelgrid_load (settings->in_filename);

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

