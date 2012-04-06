#ifndef __MESH_GENERATOR_H__
#define __MESH_GENERATOR_H__

#include "voxelgrid.h"

typedef struct MeshGenerator MeshGenerator;
typedef struct Mesh Mesh;

struct MeshGenerator
{
    unsigned int size;
    Mesh *mesh;
};

struct Mesh
{
    unsigned int num_vertices;
    unsigned int num_faces;
    float *vertices;
    unsigned int *faces;
};

Mesh *read_obj_file(const char* filename,float size);
void free_mesh(Mesh *mesh);
float trace_ray(VoxelGrid *voxelgrid, Mesh *mesh, unsigned int pos[3]);
MeshGenerator *mesh_generator_create(const char *args);
VoxelGrid *mesh_generator_generate(MeshGenerator *mesh_generator);
void mesh_generator_free(MeshGenerator *mesh_generator);

#endif
