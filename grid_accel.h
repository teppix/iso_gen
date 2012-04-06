#ifndef __GRID_ACCEL_H__
#define __GRID_ACCEL_H__

#include "mesh.h"

typedef struct Mesh Mesh;

typedef struct AccelerationGrid AccelerationGrid;

struct AccelerationGrid
{
    unsigned int size[2];
    float max_pos[2];
    unsigned int **primitives;
    unsigned int *num_primitives;
};

AccelerationGrid *acceleration_grid_create(Mesh *mesh, float max_x, float max_y, unsigned int resolution_x, unsigned int resolution_y);
void acceleration_grid_free(AccelerationGrid *acceleration_grid);

#endif
