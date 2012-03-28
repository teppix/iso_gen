#include "isogen_config.h"
#include "voxelgrid.h"
#include "settings.h"
#include "renderer.h"
#include "app.h"

int main(){
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
