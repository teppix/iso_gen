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
    // face indexo of each pixel
    unsigned int *face_map;
};

Renderer *renderer_create();
int renderer_save(Renderer *renderer, Settings *settings);
void renderer_free(Renderer *renderer);
void renderer_render(Renderer *renderer, Settings *settings, VoxelGrid *voxelgrid);
void draw_vertical_line(int x, int y, Settings *settings, Renderer *renderer, unsigned char color ,unsigned int face_id);
void draw_diagonal_line(int x, int y, int direction, Settings *settings, Renderer *renderer, unsigned char color, unsigned int face_id);
void draw_face(int x, int y, enum FaceOrientation orientation, unsigned int face_id, Settings *settings, Renderer *renderer);
void draw_numbers(Renderer *renderer, Settings *settings);
void draw_number(Renderer *renderer, unsigned int x, unsigned int y, unsigned int number);

#endif
