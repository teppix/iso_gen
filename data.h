#ifndef DATA_H
#define DATA_H

struct Data{
    // width of the image
    unsigned int image_width;
    // height of the image
    unsigned int image_height;
    // number of faces
    unsigned int num_faces;
    // number of voxels
    unsigned int num_voxels;
    // pixel values
    unsigned char *pixels;
};

#endif
