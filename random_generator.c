#include "random_generator.h"
#include "voxelgrid.h"

#include <stdlib.h>
#include <string.h>

RandomGenerator *random_generator_create(const char *args)
{
    // TODO: this is ugly, write some helper functions
    char *input_string;
    char *pch;
    RandomGenerator *random_generator = malloc(sizeof(RandomGenerator));
    // make a copy of the args ( that we are allowed to edit)
    input_string = malloc(strlen(args)*sizeof(char));
    strcpy(input_string, args);
    // - parse arguments -
    // split based on commas
    pch = strtok(input_string, ",");
    if (pch == NULL)
        return NULL;
    // read size
    random_generator->size = atoi(pch);
    // default seed 
    random_generator->seed = 0;
    // read next argument
    pch = strtok(NULL,",");
    // if there is one
    if (pch != NULL){
        // update seed
        random_generator->seed = atoi(pch);
    }
    // flush the rest of the arguments
    while(pch != NULL){
        pch = strtok(NULL,",");
    }
    return random_generator;
}

VoxelGrid *random_generator_generate(RandomGenerator *random_generator)
{
    // iterators
    unsigned int x,y,z;
    VoxelGrid *voxelgrid = voxelgrid_create(NULL,random_generator->size,random_generator->size,random_generator->size);
    // set seed
    srandom(random_generator->seed);
    // for each voxel
    for(x=0;x<voxelgrid->dim_x;x++){
        for(y=0;y<voxelgrid->dim_y;y++){
            for(z=0;z<voxelgrid->dim_z;z++){
                // randomly set it to true or false
                VOXEL(voxelgrid,x,y,z) = random()%2;
            }
        }
    }
    return voxelgrid;
}

void random_generator_free(RandomGenerator *random_generator)
{
    free(random_generator);
}
