#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "isogen_config.h"
#include "utils.h"
#include "voxelgrid.h"
#include "generator.h"
#include "settings.h"
#include "renderer.h"
#include "cli.h"
#include "gui.h"
#include "app.h"

void suggest_help(void)
{
    fprintf(stderr, "Use -h for help\n");
}

static void show_help(void)
{
    puts(
            "usage: isogen [OPTIONS] [voxel-file]\n"
            "\n"
            "Arguments:\n"
            "  -o               write image file\n"
            "  -n               draw face numbers\n"
            "  -s  radius       generate sphere\n"
            "  -r  size,[seed]  generate random voxels\n"
            "  -m  filename     generate voxels from 3d mesh\n"
            "  -v,-vv           verbosity level"
    );
}

int main(int argc, char **argv) {

    int opt;
    int error = 0;
    int result;
    int i;
    int argnum;

    // get user settings
    Settings *settings = settings_create();

    // Parse arguments
    while ( (opt = getopt(argc, argv, "hgnm:r:s:i:o:v")) != -1) {
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
            case 'o':
                strcpy (settings->out_filename, optarg);
                break;
            case 's':
                settings->generator = generator_create(GENERATOR_SPHERE,optarg);
                break;
            case 'r':
                settings->generator = generator_create(GENERATOR_RANDOM,optarg);
                break;
            case 'm':
                settings->generator = generator_create(GENERATOR_MESH,optarg);
                break;
            case 'v':
                // increment verbosity level for each 'v'
                verbosity_level++;
                break;
            case '?':
                // invalid argument
                error = 1;
                break;
        }
    }

    // unless the user specified a generator
    if(settings->generator == NULL){
        // parse remaining arguments
        for (i=optind, argnum=0; i<argc; i++, argnum++) {
            switch (argnum) {
                case 0: // == input file
                    strcpy (settings->in_filename, argv[i]);
                    break;

                default: // invalid number of arguments
                    fprintf (stderr, "Too many arguments\n");
                    error = 1;
                    break;
            }
        }
    }

    if (error != 0) {
        suggest_help();
        settings_free(settings);
        return 1;
    }

    if (settings->help_option) {
        show_help();
        settings_free(settings);
        return 0;
    }

    if (settings->gui_option) {
        result = gui_main (argc, argv, settings);
    } else {
        result = cli_main (argc, argv, settings);
    }

    // free generator
    if(settings->generator != NULL)
    {
        generator_free(settings->generator);
    }

    // free settings
    settings_free(settings);

    return result;
}
