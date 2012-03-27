#ifndef SETTINGS_H
#define SETTINGS_H

struct Settings{
    // size of the voxel grid
    unsigned int x_num_voxels;
    unsigned int y_num_voxels;
    unsigned int z_num_voxels;
    // actual voxel data
    bool *voxels;
    // the size of each face
    unsigned int face_size;
    // base color for each face
    unsigned char face_color;
};

#endif
