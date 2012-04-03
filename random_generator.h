#ifndef __RANDOM_GENERATOR_H__
#define __RANDOM_GENERATOR_H__

#include "voxelgrid.h"

typedef struct RandomGenerator RandomGenerator;

struct RandomGenerator
{
    unsigned int seed;
    unsigned int size;
};

RandomGenerator *random_generator_create(const char *args);
VoxelGrid *random_generator_generate(RandomGenerator *random_generator);
void random_generator_free(RandomGenerator *random_generator);

#endif
