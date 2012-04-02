#include <stdlib.h>

#include "generator.h"
#include "sphere_generator.h"
#include "voxelgrid.h"

Generator *generator_create(GeneratorType type,const char *args)
{
    Generator *generator = malloc(sizeof(Generator));
    generator->type = type;
    switch(generator->type){
        case GENERATOR_SPHERE:
            generator->generator_instance = sphere_generator_create(args);
            break;
    }
    // if there was an error creating the generator instance
    if(generator->generator_instance == NULL)
    {
        free(generator);
        return NULL;
    }
    return generator;
}

VoxelGrid *generator_generate(Generator* generator)
{
    switch(generator->type){
        case GENERATOR_SPHERE:
            sphere_generator_generate(generator->generator_instance);
            break;
    }
}

void generator_free(Generator* generator)
{
    switch(generator->type){
        case GENERATOR_SPHERE:
            sphere_generator_free(generator->generator_instance);
            break;
    }
}