
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "voxelgrid.h"

#define MAX(a,b) ( (a)>(b) ? (a) : (b) )


static int voxelgrid_get_dimensions (FILE *voxelgrid_file, int *dim_x, int *dim_y, int *dim_z);


VoxelGrid *voxelgrid_load(const char *filename)
{
    VoxelGrid *voxelgrid;
    FILE *voxelgrid_file;
    int dim_x, dim_y, dim_z;
    int status;

    int x,y,z;

    voxelgrid_file = fopen(filename, "r");
    status = voxelgrid_get_dimensions(voxelgrid_file, &dim_x, &dim_y, &dim_z);

    if (status) {
        fprintf(stderr, "Error reading voxelgrid dimensions from file\n");
        fclose(voxelgrid_file);
        return NULL;
    }

    voxelgrid = voxelgrid_create(filename, dim_x, dim_y, dim_z);

    // load voxels

    x = y = z = 0;

    fseek (voxelgrid_file, 0L, SEEK_SET);

    while (!feof(voxelgrid_file)) {
        int c = fgetc (voxelgrid_file);

        switch (c) {
            case '#':
                VOXEL(voxelgrid, x++, y, z) = 1;
                break;
            case ' ':
                VOXEL(voxelgrid, x++, y, z) = 0;
                break;
            case '-':
                if (y) z++;
                y = 0;
                while (!feof(voxelgrid_file) && fgetc(voxelgrid_file)!='\n') ; // flush line
                // no break here!
            case '\n':
                if (x) y++;
                x=0;
                break;
        }
    }

    fclose (voxelgrid_file);

    return voxelgrid;
}

VoxelGrid *voxelgrid_create(const char *filename, int dim_x, int dim_y, int dim_z)
{
    VoxelGrid *voxelgrid = (VoxelGrid*)malloc(sizeof(VoxelGrid));

    voxelgrid->dim_x = dim_x;
    voxelgrid->dim_y = dim_y;
    voxelgrid->dim_z = dim_z;

    if (filename == NULL)
        voxelgrid->filename[0] = '\0';
    else
        strcpy (voxelgrid->filename, filename);
    
    voxelgrid->tiles = (char *)malloc ( dim_x * dim_y * dim_z );

    return voxelgrid;
}


void voxelgrid_free (VoxelGrid *voxelgrid)
{
    if (voxelgrid->tiles != NULL)
        free (voxelgrid->tiles);
    free (voxelgrid);
}


void voxelgrid_save(VoxelGrid *voxelgrid)
{
    unsigned int x,y,z;
    FILE *file;

    fprintf (stderr, "Saving to '%s'\n", voxelgrid->filename);

    file = fopen (voxelgrid->filename, "w");

    if (file == NULL) {
        fprintf (stderr, "Could not open file '%s' for writing\n", voxelgrid->filename);
        return;
    }

    for (x=0; x<voxelgrid->dim_x; x++)
        fputc ('-', file);
    fputc ('\n', file);
    
    for (z=0; z<voxelgrid->dim_z; z++) {
        for (y=0; y<voxelgrid->dim_y; y++) {
            for (x=0; x<voxelgrid->dim_x; x++) {
                int voxel = VOXEL (voxelgrid, x, y, z);

                fputc ( voxel ? '#' : ' ' , file);
            }

            fputc ('\n', file);
        }

        // add level separator
        for (x=0; x<voxelgrid->dim_x; x++)
            fputc ('-', file);
        fputc ('\n', file);
    }

    fclose (file);
}

void voxelgrid_save_as(VoxelGrid *voxelgrid, const char *filename)
{
    strcpy (voxelgrid->filename, filename);
    voxelgrid_save (voxelgrid);
}



/*
 * read voxelgrid dimensions from open file
 */
static int voxelgrid_get_dimensions (FILE *voxelgrid_file, int *dim_x, int *dim_y, int *dim_z)
{
    if (voxelgrid_file == NULL)
        return 1;

    fseek (voxelgrid_file, 0L, SEEK_SET);

    *dim_x = *dim_y = *dim_z = 0;

    int x=0, y=0;

    while (!feof (voxelgrid_file)) {
        int c = fgetc (voxelgrid_file);

        switch(c) {
            // new tile
            case ' ':
            case '#':
                x++;
                break;

            // new level
            case '-':
                if (y) (*dim_z)++; // new level (only if any rows was encountered)
                while (!feof(voxelgrid_file) && fgetc(voxelgrid_file)!='\n') ; // flush line
                *dim_y = MAX (*dim_y, y); // update depth

                y = 0;
                // no break here!
            case '\n':
                // new line... check and update width
                *dim_x = MAX (*dim_x, x);
                if (x) y++;
                x=0;
                break;

            case EOF:
                // just ignore eof. will handle in while-loop
                break;

            default:
                // unrecognised character = ERROR...
                fprintf (stderr, "Illegal character: '%c'\n", c);
                return 1;
        }
    }
    return 0;
}

