#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <float.h>

#include "mesh_generator.h"
#include "grid_accel.h"
#include "voxelgrid.h"

#define OUTSIDE 0
#define INSIDE 1
#define MAX_LINE 128
#define BUFFER_SIZE 1024

#define MAX(a,b) ( (a)>(b) ? (a) : (b) )
#define MIN(a,b) ( (a)<(b) ? (a) : (b) )

static FILE *read_until(FILE *fp, const char *target, char *char_buffer)
{
    // position of begining of line
    fpos_t f_pos;
    unsigned int len = strlen(target);
    // store the current position in the file
    fgetpos(fp,&f_pos);
    // read line
    while(fgets(char_buffer,MAX_LINE,fp) != NULL){
        // does the current line start with our target string?
        if(strncmp(char_buffer,target,len) == 0){
            // rewind file to begining of line
            fsetpos(fp,&f_pos);
            return fp;
        }
        // store the current position in the file
        fgetpos(fp,&f_pos);
    }
    // rewind file to begining of last line
    fsetpos(fp,&f_pos);
    return fp;
}

FILE *read_obj_vertices(FILE *fp, char* char_buffer,Mesh *mesh,float size)
{
    fpos_t f_pos;
    unsigned int i,j;
    unsigned int current_vert,vert_offset;
    float rescale_factor[3];
    float **vert_buffers;
    float *vert_buffer;
    unsigned int num_buffers;
    // min and max of bounding box
    float min_pos[3] = {FLT_MAX,FLT_MAX,FLT_MAX};
    float max_pos[3] = {FLT_MIN,FLT_MIN,FLT_MIN};
    // - read vertices -
    // set number of vertices to zero
    unsigned long num_verts=0;

    // store first vertex buffer
    vert_buffers = (float **)malloc(sizeof(float *));
    // allocate first vertex buffer
    vert_buffers[0] = (float *)malloc(3*BUFFER_SIZE*sizeof(float));
    num_buffers = 1;
    // set up pointer to current buffer
    vert_buffer = vert_buffers[0];

    fgetpos(fp,&f_pos);
    // as long as the current line starts with "v "
    while(fgets(char_buffer,MAX_LINE,fp) != NULL && strncmp(char_buffer,"v ",2) == 0){
        char *current_buffer;
        char *next_char;
        // if we've filled the buffer
        if(num_verts == BUFFER_SIZE){
            float **tmp_buffer_p;
            // allocate space for a new array of pointers
            tmp_buffer_p = vert_buffers;
            vert_buffers = (float **)malloc((num_buffers+1)*sizeof(float *));
            // copy previous pointers to new array
            memcpy(vert_buffers, tmp_buffer_p, (num_buffers)*sizeof(float*));
            // free previous array
            free(tmp_buffer_p);
            // allocate space for a new buffer
            vert_buffers[num_buffers] = (float *)malloc(3*BUFFER_SIZE*sizeof(float));
            // update pointer
            vert_buffer = vert_buffers[num_buffers];
            // update buffer counter
            num_buffers++;
            // reset number of vertices
            num_verts =0;
        }
        // - get data from line -
        // TODO: handle errors
        // read x coordinate
        current_buffer = char_buffer +2;
        vert_buffer[num_verts*3] = strtof(current_buffer,&next_char);
        // read y coordinate
        current_buffer = next_char;
        vert_buffer[num_verts*3+1] = strtof(current_buffer,&next_char);
        // read z coordinate
        current_buffer = next_char;
        vert_buffer[num_verts*3+2] = strtof(current_buffer,&next_char);

        // update bounding box
        min_pos[0] = MIN(vert_buffer[num_verts*3],min_pos[0]);
        max_pos[0] = MAX(vert_buffer[num_verts*3],max_pos[0]);
        min_pos[1] = MIN(vert_buffer[num_verts*3+1],min_pos[1]);
        max_pos[1] = MAX(vert_buffer[num_verts*3+1],max_pos[1]);
        min_pos[2] = MIN(vert_buffer[num_verts*3+2],min_pos[2]);
        max_pos[2] = MAX(vert_buffer[num_verts*3+2],max_pos[2]);

        // update number of vertices
        num_verts++;
        // store current position in file
        fgetpos(fp,&f_pos);
    }
    // rewind file to begining of last line
    fsetpos(fp,&f_pos);
    // - copy vertex data to mesh -
    mesh->num_vertices = num_verts+ BUFFER_SIZE*(num_buffers-1);
    mesh->vertices = malloc(3*mesh->num_vertices*sizeof(float));
    // find rescale factor
    for(i=0;i<3;i++)
    {
        rescale_factor[i] = 1.0f/(max_pos[i] - min_pos[i])*size;
    }
    current_vert = 0;
    // copy vertex data
    for(i=0;i<num_buffers;i++)
    {
        vert_offset = current_vert-i*BUFFER_SIZE;
        while(current_vert < mesh->num_vertices && (vert_offset)<BUFFER_SIZE){
            // rescale coordinates to range 0 - size
            for(j=0;j<3;j++)
            {
                mesh->vertices[current_vert*3+j] = (vert_buffers[i][(vert_offset)*3+j]-min_pos[j])*rescale_factor[j];
            }
            current_vert++;
            vert_offset = current_vert-i*BUFFER_SIZE;
        }
    }
    // free buffers
    for(i=0;i<num_buffers;i++)
    {
        free(vert_buffers[i]);
    }
    free(vert_buffers);
    // return
    return fp;
}

FILE *read_obj_faces(FILE *fp, char* char_buffer, Mesh *mesh)
{
    unsigned int i,j;
    fpos_t f_pos;
    char *current_buffer;
    char *next_char;
    unsigned int current_face,face_offset;
    unsigned int **face_buffers;
    unsigned int *face_buffer;
    unsigned int num_buffers;
    // - read faces -
    // set number of faces to zero
    unsigned long num_faces = 0;

    // store first face buffer
    face_buffers = (unsigned int **)malloc(sizeof(unsigned int *));
    // allocate first face buffer
    face_buffers[0] = (unsigned int *)malloc(3*BUFFER_SIZE*sizeof(unsigned int));
    num_buffers = 1;
    // set up pointer to current buffer
    face_buffer = face_buffers[0];

    // store current position in file
    fgetpos(fp,&f_pos);
    while(fgets(char_buffer,MAX_LINE,fp) != NULL && strncmp(char_buffer,"f ",2) == 0){
        // if we've filled the buffer
        if(num_faces == BUFFER_SIZE){
            unsigned int **tmp_buffer_p;
            // allocate space for a new array of pointers
            tmp_buffer_p = face_buffers;
            face_buffers = (unsigned int **)malloc((num_buffers+1)*sizeof(unsigned int *));
            // copy previous pointers to new array
            memcpy(face_buffers, tmp_buffer_p, (num_buffers)*sizeof(unsigned int*));
            // free previous array
            free(tmp_buffer_p);
            // allocate space for a new buffer
            face_buffers[num_buffers] = (unsigned int *)malloc(3*BUFFER_SIZE*sizeof(unsigned int));
            // update pointer
            face_buffer = face_buffers[num_buffers];
            // update buffer counter
            num_buffers++;
            // reset number of vertices
            num_faces =0;
        }
        // - get data from line -
        // TODO: handle errors
        // read x coordinate
        current_buffer = char_buffer +2;
        face_buffer[num_faces*3] = strtol(current_buffer,&next_char,10)-1;
        // read y coordinate
        current_buffer = next_char;
        face_buffer[num_faces*3+1] = strtol(current_buffer,&next_char,10)-1;
        // read z coordinate
        current_buffer = next_char;
        face_buffer[num_faces*3+2] = strtol(current_buffer,&next_char,10)-1;
        // update number of vertices
        num_faces++;
        // read the next line
        // store current position in file
        fgetpos(fp,&f_pos);
    }
    // rewind file to begining of last line
    fsetpos(fp,&f_pos);
    // - copy face data to mesh -
    mesh->num_faces = num_faces+BUFFER_SIZE*(num_buffers-1);
    mesh->faces = malloc(3*mesh->num_faces*sizeof(float));
    current_face = 0;
    // copy face data
    for(i=0;i<num_buffers;i++)
    {
        face_offset = current_face-i*BUFFER_SIZE;
        while(current_face < mesh->num_faces && (face_offset)<BUFFER_SIZE){
            for(j=0;j<3;j++)
            {
                mesh->faces[current_face*3+j] = face_buffers[i][(face_offset)*3+j];
            }
            current_face++;
            face_offset = current_face-i*BUFFER_SIZE;
        }
    }
    // free buffers
    for(i=0;i<num_buffers;i++)
    {
        free(face_buffers[i]);
    }
    free(face_buffers);
    // return
    return fp;
}

Mesh *read_obj_file(const char* filename,float size)
{
    unsigned int i,j;
    char char_buffer[MAX_LINE];
    // open file
    FILE *fp = fopen(filename,"r");
    if(fp == NULL){
        log_printf(0,"Error: file \"%s\" does not exist!\n",filename);
        return NULL;
    }
    // create mesh
    Mesh *mesh = (Mesh *)malloc(sizeof(Mesh));
    // find definition of vertices 
    fp = read_until(fp,"v ", char_buffer);
    // read vertices
    fp = read_obj_vertices(fp,char_buffer,mesh,size);
    // find definition of faces 
    fp = read_until(fp,"f ", char_buffer);
    // read faces
    fp = read_obj_faces(fp,char_buffer,mesh);
    // close file
    fclose(fp);
    // report results
    log_printf(1, "Read %d vertices and %d faces from file %s\n",mesh->num_vertices,mesh->num_faces,filename);
    // return mesh
    return mesh;
}

void free_mesh(Mesh *mesh)
{
    if(mesh != NULL){
        if(mesh->vertices != NULL){
            free(mesh->vertices);
        }
        if(mesh->faces != NULL){
            // free faces
            free(mesh->faces);
        }
        free(mesh);
    }
}

static void bubble_sort(unsigned int *elements, unsigned int num_elements)
{
    // simple bubble sort implementation
    unsigned int i;
    unsigned int tmp;
    char swapped;
    do
    {
        swapped = 0;
        for(i=1;i<num_elements;i++){
            if(elements[i-1] > elements[i]){
                tmp = elements[i];
                elements[i] = elements[i-1];
                elements[i-1] = tmp;
                swapped = 1;
            }
        }
    }while (swapped);
}

void free_intersections(unsigned int *intersections)
{
    if(intersections != NULL)
    {
        free(intersections);
    }
}

float get_intersections(VoxelGrid *voxelgrid, Mesh *mesh, AccelerationGrid *acceleration_grid, unsigned int pos[3], unsigned int **intersections)
{
    unsigned int i;
    unsigned int num_faces;
    unsigned int *faces;
    unsigned int current_face;
    unsigned int num_intersections;
    // get triangles to test
    num_faces = acceleration_grid_lookup(acceleration_grid,pos[0],pos[1],&faces);
    // allocate space for intersection data
    if(*intersections != NULL)
    {
        free(*intersections);
    }
    // NOTE: we might allocate more memory than we'll be using (all faces might not be intersected)
    // probably shouldn't be a problem
    *intersections = (unsigned int *)malloc(num_faces*sizeof(int));
    // set number of intersections to zero
    num_intersections = 0;
    // for each triangle
    for(i =0;i<num_faces;i++)
    {
        unsigned int vert,coord;
        float determinant;
        float bary_coords[3];
        float tri_coords[3][3];
        current_face = faces[i];
        // get the coordinates of the triangle
        for(vert=0;vert<3;vert++){
            for(coord=0;coord<3;coord++){
                tri_coords[vert][coord] = mesh->vertices[mesh->faces[current_face*3+vert]*3+coord];
            }
        }
        // -- find out if point lies inside triangle in the x-y plane --
        // - get barycentric coordinates for the point -
        // (could come in handy later as well)
        determinant = (tri_coords[1][1] - tri_coords[2][1])*(tri_coords[0][0] - tri_coords[2][0]) +
                        (tri_coords[2][0] - tri_coords[1][0])*(tri_coords[0][1] - tri_coords[2][1]);
        bary_coords[0] = (tri_coords[1][1] - tri_coords[2][1])*(pos[0] - tri_coords[2][0]) + \
                                (tri_coords[2][0] - tri_coords[1][0])*(pos[1] - tri_coords[2][1]);
        bary_coords[0] /= determinant;
        bary_coords[1] = (tri_coords[2][1] - tri_coords[0][1])*(pos[0] - tri_coords[2][0]) + \
                                (tri_coords[0][0] - tri_coords[2][0])*(pos[1] - tri_coords[2][1]);
        bary_coords[1] /= determinant;
        bary_coords[2] = 1 - bary_coords[0] - bary_coords[1];
        // if there's an intersection (point is inside triangle)
        if(bary_coords[0] > 0 && bary_coords[0] < 1){
            if(bary_coords[1] > 0 && bary_coords[1] < 1){
                if(bary_coords[2] > 0 && bary_coords[2] < 1){
                    // get depth
                    (*intersections)[num_intersections] = bary_coords[0]*tri_coords[0][2]+bary_coords[1]*tri_coords[1][2]+bary_coords[2]*tri_coords[2][2];
                    num_intersections++;
                }
            }
        }
    }
    // sort intersections
    bubble_sort(*intersections,num_intersections);
    
    return num_intersections;
}


MeshGenerator *mesh_generator_create(const char *args)
{
    // create generator
    MeshGenerator *mesh_generator = (MeshGenerator *)malloc(sizeof(MeshGenerator));
    // TODO: get size from user
    mesh_generator->size = 40;
    // read mesh file
    mesh_generator->mesh = read_obj_file(args,(float)mesh_generator->size);
    return mesh_generator;
}

VoxelGrid *mesh_generator_generate(MeshGenerator *mesh_generator)
{
    float distance;
    unsigned int pos[3];
    unsigned int num_intersections;
    unsigned int intersection;
    unsigned int *intersections = NULL;
    // create voxel grid
    VoxelGrid *voxelgrid = voxelgrid_create(NULL,mesh_generator->size,mesh_generator->size,mesh_generator->size);
    // create acceleration structure
    AccelerationGrid *acceleration_grid = acceleration_grid_create(mesh_generator->mesh, mesh_generator->size,mesh_generator->size,mesh_generator->size,mesh_generator->size);
    // current state (inside or outside model)
    unsigned int state = OUTSIDE;
    if(mesh_generator->mesh != NULL){
        // for each line of voxels
        for(pos[0]=0;pos[0]<voxelgrid->dim_x;pos[0]++){
            for(pos[1]=0;pos[1]<voxelgrid->dim_y;pos[1]++){
                // set z coordinate to zero
                pos[2] = 0;
                // reset state to outside
                state = OUTSIDE;
                // get intersections
                num_intersections = get_intersections(voxelgrid, mesh_generator->mesh, acceleration_grid, pos,&intersections);
                // for each intersection
                for(intersection=0;intersection<num_intersections;intersection++){
                    unsigned int i;
                    // for each voxel along the way
                    while(pos[2]<intersections[intersection]){
                        // update density
                        // TODO: we shouldn't have to flip z
                        VOXEL(voxelgrid,pos[0],pos[1],mesh_generator->size -pos[2]-1) = state;
                        // update z coordinate
                        pos[2]++;
                    }
                    // toggle state
                    state = 1-state;
                }
            }
        }
    }
    // free intersection data
    free_intersections(intersections);
    // free acceleration structure
    acceleration_grid_free(acceleration_grid);
    // return voxel grid
    return voxelgrid;
}

void mesh_generator_free(MeshGenerator *mesh_generator)
{
    // free mesh
    free_mesh(mesh_generator->mesh);
    mesh_generator->mesh = NULL;
    free(mesh_generator);
}
