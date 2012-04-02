#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include "voxelgrid.h"

typedef enum GeneratorType GeneratorType;

enum GeneratorType
{
    GENERATOR_SPHERE
};

typedef struct Generator Generator;

struct Generator
{
    GeneratorType type;
    void *generator_instance;
};

Generator *generator_create(GeneratorType type,const char* args);
VoxelGrid *generator_generate();
void generator_free();

#endif
