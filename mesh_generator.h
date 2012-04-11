#ifndef __MESH_GENERATOR_H__
#define __MESH_GENERATOR_H__

#include "voxelgrid.h"
#include "grid_accel.h"
#include "mesh.h"

typedef struct MeshGenerator MeshGenerator;

struct MeshGenerator
{
    unsigned int size;
    Mesh *mesh;
};


Mesh *read_obj_file(const char* filename,float size);
void free_mesh(Mesh *mesh);
void free_intersections(unsigned int *intersections);
float get_intersections(VoxelGrid *voxelgrid, Mesh *mesh, AccelerationGrid *acceleration_grid, unsigned int pos[3], unsigned int **intersections);
MeshGenerator *mesh_generator_create(const char *args);
VoxelGrid *mesh_generator_generate(MeshGenerator *mesh_generator);
void mesh_generator_free(MeshGenerator *mesh_generator);

#endif
