#ifndef __RENDERER_H__
#define __RENDERER_H__
#include <stdint.h>
#include <cairo.h>

#define COLOR(r,g,b) ((((r & 0xff) << 16 ) | ((g & 0xff) << 8)) | (b&0xff))

// the different face directions
enum FaceOrientation{
    FACE_LEFT,
    FACE_RIGHT,
    FACE_TOP
};

typedef struct Renderer Renderer;

struct Renderer{
    // width of the image
    unsigned int image_width;
    // height of the image
    unsigned int image_height;
    // stride (number of pixels in each row
    unsigned int stride;
    // number of faces
    unsigned int num_faces;
    // number of voxels
    unsigned int num_voxels;
    // temporary storage of pixels
    int32_t *pixels;
    // cairo surface
    cairo_surface_t *surface;
    // face indexo of each pixel
    unsigned int *face_map;
};


Renderer *renderer_create();
int renderer_save(Renderer *renderer, Settings *settings);
void renderer_free(Renderer *renderer);
void renderer_render(Renderer *renderer, Settings *settings, VoxelGrid *voxelgrid);
void draw_vertical_line(int x, int y, Settings *settings, Renderer *renderer, int32_t color ,unsigned int face_id);
void draw_diagonal_line(int x, int y, int direction, Settings *settings, Renderer *renderer, int32_t color, unsigned int face_id);
void draw_face(int x, int y, enum FaceOrientation orientation, unsigned int face_id, Settings *settings, Renderer *renderer);
void draw_numbers(Renderer *renderer, Settings *settings);
void draw_number(Renderer *renderer, unsigned int x, unsigned int y, unsigned int number);

#endif
