#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#define MAX_FILE_NAME 1024

#include "color.h"
#include "generator.h"

typedef struct Settings Settings;

struct Settings{
    unsigned int help_option;
    unsigned int gui_option;
    // the size of each face
    unsigned int face_size;
    // base color for each face
    Color face_color;
    // color of the outlines
    Color outline_color;
	// light level in each direction
	unsigned char light[3];
    // should we draw numbers on the faces?
    unsigned char numbered_faces;
    // should we draw outlines?
    unsigned char outlines;
    // the generator to use
    Generator *generator;
	// filename of output file
    char in_filename[MAX_FILE_NAME];
    char out_filename[MAX_FILE_NAME];
};

Settings *settings_create();
void settings_free(Settings *settings);

#endif
