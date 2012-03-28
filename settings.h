#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#define MAX_FILE_NAME 1024

typedef struct Settings Settings;

struct Settings{
    // the size of each face
    unsigned int face_size;
    // base color for each face
    unsigned char face_color;
	// light level in each direction
	unsigned char light[3];
    // should we draw numbers on the faces?
    unsigned char numbered_faces;
	// filename of output file
    char out_filename[MAX_FILE_NAME];
};

Settings *settings_create();
void settings_free(Settings *settings);

#endif
