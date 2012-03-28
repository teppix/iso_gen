#ifndef __VOXELGRID_H__
#define __VOXELGRID_H__

#define MAX_FILE_NAME 1024

#define VOXEL(vg, x, y, z) *((vg)->tiles + (x) + (y)*(vg)->dim_x + (z)*(vg)->dim_y*(vg)->dim_x)

typedef struct VoxelGrid VoxelGrid;

struct VoxelGrid
{
    unsigned int dim_x;
    unsigned int dim_y;
    unsigned int dim_z;

    char filename[MAX_FILE_NAME];
    char *tiles;
};



VoxelGrid *voxelgrid_load(const char *filename);
VoxelGrid *voxelgrid_create(const char *filename, int dim_x, int dim_y, int dim_z);
void voxelgrid_free (VoxelGrid *voxelgrid);

void voxelgrid_save(VoxelGrid *voxelgrid);
void voxelgrid_save_as(VoxelGrid *voxelgrid, const char *filename);

#endif // __VOXELGRID_H__
