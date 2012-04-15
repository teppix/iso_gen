#ifndef __MESH_H__
#define __MESH_H__

typedef struct Mesh Mesh;

struct Mesh
{
    unsigned int num_vertices;
    unsigned int num_faces;
    float *vertices;
    unsigned int *faces;
};

#endif
