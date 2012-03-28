#include <stdlib.h>
#include <string.h>

#include "settings.h"

Settings *settings_create(){
    Settings *settings = (Settings*)malloc(sizeof(Settings));
    // color of the faces
    settings->face_color = 160;
	// light levels
	settings->light[0] = 0;
	settings->light[1] = 20;
	settings->light[2] = 50;
    // face size (in screen space)
    // - each voxel will be (face_size+1)*2 pixels wide -
    // - and face_size*1.5+a pixels high -
    settings->face_size = 18;
    // name of our output file
    strcpy(settings->out_filename,"out.png");
    return settings;
}
void settings_free(Settings *settings){
    // free the settings
    free(settings);
}
