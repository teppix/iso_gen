#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "isogen_config.h"
#include "voxelgrid.h"
#include "settings.h"
#include "renderer.h"
#include "app.h"

static void exit_callback(void)
{
    fprintf(stderr, "Use -h for help\n");
}

static void show_help(void)
{
    puts(
            "usage: isogen [OPTION]\n"
            "\n"
            "Arguments:\n"
            "  -i           voxel file to load\n"
            "  -o           write image file"
    );
}

static int main_cli (int argc, char **argv, Settings *settings)
{
    // check input filename
    if (strlen(settings->in_filename) == 0) {
        fprintf (stderr, "Error: No input file specified\n");
        exit(1);
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


static int main_gui (int argc, char **argv, Settings *settings)
{
    printf ("Gui not implemented yet\n");
    return 0;
}


int main(int argc, char **argv) {

    int opt;
    int error = 0;
    int result;


    atexit(exit_callback);

    // get user settings
    Settings *settings = settings_create();

    // Parse arguments
    while ( (opt = getopt(argc, argv, "hgni:o:")) != -1) {
        switch (opt) {
            case 'h':
                settings->help_option = 1;
                break;
            case 'g':
                settings->gui_option = 1;
                break;
            case 'n':
                settings->numbered_faces = 1;
                break;
            case 'i':
                strcpy (settings->in_filename, optarg);
                break;
            case 'o':
                strcpy (settings->out_filename, optarg);
                break;
            case '?':
                // invalid argument
                error = 1;
                break;
        }
    }

    if (error != 0) {
        exit(1);
    }

    if (settings->help_option) {
        show_help();
        return 0;
    }

    if (settings->gui_option) {
        result = main_gui (argc, argv, settings);
    } else {
        result = main_cli (argc, argv, settings);
    }

    // free settings
    settings_free(settings);

    return result;
}
