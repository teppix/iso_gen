#include <stdlib.h>
#include <math.h>

#include "sphere_generator.h"
#include "voxelgrid.h"

SphereGenerator *sphere_generator_create(const char *args)
{
    // TODO: parse arguments
    SphereGenerator *sphere_generator = malloc(sizeof(SphereGenerator));
    sphere_generator->radius = atoi(args);
    if(sphere_generator->radius == 0)
    {
        log_printf(0, "Error: invalid radius\n");
        free(sphere_generator);
        return NULL;
    }
    return sphere_generator;
}

VoxelGrid *sphere_generator_generate(SphereGenerator *sphere_generator)
{
    // iterators
    unsigned int x,y,z;
    VoxelGrid *voxelgrid = voxelgrid_create(NULL,sphere_generator->radius*2,sphere_generator->radius*2,sphere_generator->radius*2);
    // for each voxel
    for(x=0;x<voxelgrid->dim_x;x++){
        for(y=0;y<voxelgrid->dim_y;y++){
            for(z=0;z<voxelgrid->dim_z;z++){
                // get the distance to the center
                float dist_x = fabs(0.5+(float)x-sphere_generator->radius);
                float dist_y = fabs(0.5+(float)y-sphere_generator->radius);
                float dist_z = fabs(0.5+(float)z-sphere_generator->radius);
                float distance = sqrtf(dist_y*dist_y+dist_x*dist_x);
                distance = sqrtf(dist_z*dist_z+distance*distance);
                // if the distance is less than the radius
                if(distance < sphere_generator->radius)
                {
                    // set voxel to true
                    VOXEL(voxelgrid,x,y,z) = 1;
                }
            }
        }
    }
    return voxelgrid;
}

void sphere_generator_free(SphereGenerator *sphere_generator)
{
    free(sphere_generator);
}
