#ifndef __RENDERER_H__
#define __RENDERER_H__

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
    // number of faces
    unsigned int num_faces;
    // number of voxels
    unsigned int num_voxels;
    // pixel values
    unsigned char *pixels;
};

Renderer *renderer_create();
int renderer_save(Renderer *renderer, Settings *settings);
void renderer_free(Renderer *renderer);
void renderer_render(Renderer *renderer, Settings *settings, VoxelGrid *voxelgrid);
void draw_vertical_line(int x, int y, Settings *settings, Renderer *renderer, unsigned char color);
void draw_diagonal_line(int x, int y, int direction, Settings *settings, Renderer *renderer, unsigned char color);
void draw_face(int x, int y, enum FaceOrientation orientation, unsigned char color, Settings *settings, Renderer *renderer);
void draw_numbers(Renderer *renderer, Settings *settings);

#endif
