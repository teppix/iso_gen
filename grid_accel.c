#include "grid_accel.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX(a,b) ( (a)>(b) ? (a) : (b) )
#define MIN(a,b) ( (a)<(b) ? (a) : (b) )

static unsigned int *add_primitive_to_list(unsigned int *list, unsigned int len, unsigned int face_id){
    unsigned int *tmp_list = list;
    // TODO: reduce the number of malloc's
    list = (unsigned int*)malloc((len+1)*sizeof(int));
    if(tmp_list != NULL){
        memcpy(list,tmp_list,len*sizeof(int));
        free(tmp_list);
    }
    list[len] = face_id;
    return list;
}

unsigned int acceleration_grid_lookup(AccelerationGrid *acceleration_grid, unsigned int pos_x, unsigned int pos_y, unsigned int **primitives)
{
    unsigned int index = pos_x + pos_y*acceleration_grid->size[0];
    *primitives = acceleration_grid->primitives[index];
    return acceleration_grid->num_primitives[index];
}

AccelerationGrid *acceleration_grid_create(Mesh *mesh, float max_x, float max_y, unsigned int resolution_x, unsigned int resolution_y)
{
    unsigned int x,y;
    unsigned int index;
    unsigned int face;
    unsigned int num_cells;
    unsigned int **primitives;
    unsigned int *num_primitives;
    AccelerationGrid *acceleration_grid = (AccelerationGrid *)malloc(sizeof(AccelerationGrid));
    acceleration_grid->size[0] = resolution_x;
    acceleration_grid->size[1] = resolution_y;
    num_cells = acceleration_grid->size[0]*acceleration_grid->size[1];
    acceleration_grid->max_pos[0] = max_x;
    acceleration_grid->max_pos[1] = max_y;
    primitives = (unsigned int **)malloc(num_cells*sizeof(int *));
    memset(primitives,0,num_cells*sizeof(int*));
    num_primitives = (unsigned int *)malloc(num_cells*sizeof(int));
    memset(num_primitives,0,num_cells*sizeof(int));
    // for each face
    for(face=0;face<mesh->num_faces;face++){
        unsigned int vert,coord;
        unsigned int min_coord[2];
        unsigned int max_coord[2];
        float tri_coords[3][2];
        // get the coordinates of the triangle
        for(vert=0;vert<3;vert++){
            for(coord=0;coord<2;coord++){
                tri_coords[vert][coord] = mesh->vertices[mesh->faces[face*3+vert]*3+coord];
            }
        }
        // get bounding box
        min_coord[0] = floor(MIN(tri_coords[0][0],MIN(tri_coords[1][0],tri_coords[2][0])));
        min_coord[1] = floor(MIN(tri_coords[0][1],MIN(tri_coords[1][1],tri_coords[2][1])));
        max_coord[0] = ceil(MAX(tri_coords[0][0],MAX(tri_coords[1][0],tri_coords[2][0])));
        max_coord[1] = ceil(MAX(tri_coords[0][1],MAX(tri_coords[1][1],tri_coords[2][1])));
        // for each step in x
        for(x=min_coord[0];x<max_coord[0];x++){
            // for each step in y
            for(y=min_coord[1];y<max_coord[1];y++){
                index = x+y*acceleration_grid->size[0];
                // update grid
                primitives[index] = add_primitive_to_list(primitives[index],
                        num_primitives[index], face);
                num_primitives[index]++;
            }
        }
    }
    acceleration_grid->primitives = primitives;
    acceleration_grid->num_primitives = num_primitives;
    return acceleration_grid;
}

void acceleration_grid_free(AccelerationGrid *acceleration_grid)
{
    unsigned int i;
    for(i=0; i<acceleration_grid->size[0]*acceleration_grid->size[1];i++){
        free(acceleration_grid->primitives[i]);
    }
    free(acceleration_grid->primitives);
    free(acceleration_grid->num_primitives);
    free(acceleration_grid);
}
