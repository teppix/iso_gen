#ifndef __SPHERE_GENERATOR__
#define __SPHERE_GENERATOR__

#include "voxelgrid.h"

typedef struct SphereGenerator SphereGenerator;

struct SphereGenerator
{
    unsigned int radius;
};

SphereGenerator *sphere_generator_create(const char *args);
VoxelGrid *sphere_generator_generate(SphereGenerator *sphere_generator);
void sphere_generator_free(SphereGenerator *sphere_generator);

#endif
