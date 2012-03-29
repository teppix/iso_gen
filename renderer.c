#include <string.h>
#include <stdlib.h>
#include <png.h>

#include "utils.h"
#include "settings.h"
#include "voxelgrid.h"
#include "renderer.h"
#include "font.xbm"

int renderer_save(Renderer *renderer, Settings *settings){
    // iterator
    unsigned int i;
    log_printf(1, "writing image file: %s\n", settings->out_filename);
    // open file
    FILE *fp = fopen(settings->out_filename,"wb");
    // check that the file was opened ok
    if(!fp){
        return 1;
    }
    // initialize libpng
    // TODO: error checking
    // the two main structures
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL,NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);
    // if we couldn't create the info pointer
    if(!info_ptr){
        // destroy the png pointer and return
        png_destroy_write_struct(&png_ptr,(png_infopp)NULL);
        return 1;
    }
    // initialize libpng io
    png_init_io(png_ptr,fp);
    // set file info
    // TODO: write greyscale?
    png_set_IHDR(png_ptr,info_ptr,renderer->image_width, renderer->image_height,8,PNG_COLOR_TYPE_RGB,
            PNG_INTERLACE_NONE,PNG_COMPRESSION_TYPE_DEFAULT,PNG_FILTER_TYPE_DEFAULT);
    // - hand pixels to libpng -
    // array of row pointers
    png_bytep *row_pointers = (png_bytep*)malloc(renderer->image_height*sizeof(png_bytep));
    // for each row in image
    for(i=0;i<renderer->image_height;i++){
        // point row pointer to correct value
        // note: inverted since its been drawn upside down
        row_pointers[i] = renderer->pixels+(renderer->image_height-i-1)*3*renderer->image_width;
    }
    // register row pointers with libpng
    png_set_rows(png_ptr, info_ptr, row_pointers);
    // write file to disk
    png_write_png(png_ptr,info_ptr,PNG_TRANSFORM_IDENTITY,NULL);
    // free libpng
    png_destroy_write_struct(&png_ptr,&info_ptr);
    // close file
    fclose(fp);
    // free row pointers
    free(row_pointers);
    // return success
    return 0;
}
Renderer *renderer_create(){
    Renderer *renderer = (Renderer*)malloc(sizeof(Renderer));
    // set pointers to NULL to note that they're unallocated
    renderer->pixels = NULL;
    renderer->face_map = NULL;
    return renderer;
}

void renderer_free(Renderer *renderer){
    // free pixels
    if(renderer->pixels != NULL){
        free(renderer->pixels);
    }
    if(renderer->face_map != NULL){
        // free face map
        free(renderer->face_map);
    }
    free(renderer);
}

void renderer_render(Renderer *renderer, Settings *settings, VoxelGrid *voxelgrid){
    unsigned x,y,z;
    unsigned int origin_x;
    unsigned int origin_y;
    unsigned char face_id;
    unsigned int *face_map;
    // free previous pixels
    // TODO: reuse if image size hasn't changed
    if(renderer->pixels != NULL){
        free(renderer->pixels);
    }
    // final image height (padded by one voxel)
    renderer->image_height = (voxelgrid->dim_x+voxelgrid->dim_y+2*voxelgrid->dim_z+1)*settings->face_size/2;
    // final image width (padded by one voxel)
    renderer->image_width = settings->face_size*(voxelgrid->dim_x+voxelgrid->dim_y+1);
    // where to place the 0,0,0 voxel
    origin_x = voxelgrid->dim_x*settings->face_size + settings->face_size/2;
    origin_y = (voxelgrid->dim_x+voxelgrid->dim_y-2)*settings->face_size/2+ settings->face_size/4;
    // pixels for final image
    renderer->pixels = (unsigned char*)malloc((3*renderer->image_height*renderer->image_width)*sizeof(char));
    // set all pixels to black
    memset(renderer->pixels,255,3*renderer->image_height*renderer->image_width*sizeof(char));
    // id of the currently drawn face
    face_id = 1;
    // free previous face map
    // TODO: reuse if the image size hasn't changed
    if(renderer->face_map != NULL){
        // free face map
        free(renderer->face_map);
    }
    // store the face index of each pixel
    renderer->face_map = (unsigned int*)malloc(renderer->image_height*renderer->image_width*sizeof(unsigned int));
    // set all face indices to 0 (no face)
    memset(renderer->face_map,0,renderer->image_height*renderer->image_width*sizeof(int));
    // for each voxel 
    for(x=0;x<voxelgrid->dim_x;x++){
        for(y=0;y<voxelgrid->dim_y;y++){
            for(z=0;z<voxelgrid->dim_z;z++){
                // if the current voxel is filled
                if (VOXEL (voxelgrid, x, y, z) == 1) {
                    // TODO: flip x axis
                    // placement relative to other voxels
                    int offset_x = y-x;
                    int pos_x = origin_x+(offset_x)*(settings->face_size);
                    int offset_y = -x-y+2*z;
                    int pos_y = origin_y+(offset_y)*(settings->face_size/2);
                    // -- left face --
                    // - check visibility (to avoid increasing the id for faces not visible) -
                    // should we draw the face?
                    char face_visible = 1;
                    // unless we're on the far left side
                    if(x<voxelgrid->dim_x-1) {
                        // check if there's a voxel directly in front of the left face
                        if(VOXEL(voxelgrid, x+1, y, z)==1){
                            // don't draw the face (will not be visible anyway)
                            face_visible=0;
                        }
                    }
                    // if there's nothing blocking
                    if(face_visible){
                        // draw left face
                        draw_face(pos_x,pos_y,FACE_LEFT,face_id, settings, renderer);
                        // increase face id
                        face_id++;
                    }
                    // -- right face --
                    // - check visibility (to avoid increasing the id for faces not visible) -
                    face_visible = 1;
                    if(y<voxelgrid->dim_y-1) {
                        // check if there's a voxel directly in front of the left face
                        if(VOXEL(voxelgrid, x, y+1, z)==1) {
                            // don't draw the face (will not be visible anyway)
                            face_visible=0;
                        }
                    }
                    // if there's nothing blocking
                    if(face_visible){
                        // draw right face
                        draw_face(pos_x,pos_y,FACE_RIGHT,face_id, settings, renderer);
                        // increase face id
                        face_id++;
                    }
                    // -- top face --
                    // - check visibility (to avoid increasing the id for faces not visible) -
                    face_visible = 1;
                    if(z<voxelgrid->dim_z-1)
                    {
                        // check if there's a voxel directly in front of the left face
                        if(VOXEL(voxelgrid, x, y, z+1)==1)
                        {
                            // don't draw the face (will not be visible anyway)
                            face_visible=0;
                        }
                    }
                    // if there's nothing blocking
                    if(face_visible){
                        // draw top face
                        draw_face(pos_x,pos_y,FACE_TOP,face_id, settings, renderer);
                        // increase face id
                        face_id++;
                    }
                }
            }
        }
    }
    // set number of faces
    renderer->num_faces = face_id;
    // draw the number of each face (if the user wants us to)
    if(settings->numbered_faces){
        draw_numbers(renderer, settings);
    }
}

void draw_numbers(Renderer *renderer, Settings *settings){
    // iterators
    unsigned int x,y,face;
    // - store the min and max coordinates for each face -
    //
    // minimum x coordinate
    unsigned int *accum_x = (unsigned int*)malloc(renderer->num_faces*sizeof(int));
    unsigned int *accum_y = (unsigned int*)malloc(renderer->num_faces*sizeof(int));
    // number of pixels per face
    unsigned int *count = (unsigned int*)malloc(renderer->num_faces*sizeof(int));
    // which number to draw on each face
    unsigned int *face_number = (unsigned int*)malloc(renderer->num_faces*sizeof(int));

    // initialize all coordinates to zero
    memset(accum_y,0,renderer->num_faces*sizeof(int));
    memset(accum_x,0,renderer->num_faces*sizeof(int));

    // initialize count to zero
    memset(count,0,renderer->num_faces*sizeof(int));

    // initialize face numbers to zero
    memset(face_number,0,renderer->num_faces*sizeof(int));

    // start with zero as current face
    face =0;

    // for each pixel
    for(y=0;y<renderer->image_height;y++){
        for(x=0;x<renderer->image_width;x++){
            // get the pixel value
            unsigned int face_id = renderer->face_map[x+renderer->image_width*y];
            // if it's not the background
            if(face_id != 0){
                // find which face it is
                face_id--;
                // update coordinate
                accum_x[face_id] += x;
                accum_y[face_id] += y;
                // update pixel count
                count[face_id]++;
                // if this is the first time we've encountered this face
                if(count[face_id]==1){
                    // update face number
                    face_number[face_id] = face;
                    face++;
                }
            }
        }
    }
    // for each face
    for(face=0;face<renderer->num_faces;face++){
        // if we've found it
        if(count[face]!=0){
            // draw the number in the middle of each face
            // middle x pos
            unsigned middle_x = accum_x[face]/count[face];
            // middle y pos
            unsigned middle_y = accum_y[face]/count[face];
            draw_number(renderer, middle_x, middle_y,face_number[face]);
        }
    }
    // free our arrays
    free(accum_x);
    free(accum_y);
    free(count);
}

void draw_face(int x, int y, enum FaceOrientation orientation, unsigned int face_id, Settings *settings, Renderer *renderer){
    // used for filling faces
    unsigned x_pos = 1;
    unsigned y_pos = 1;
    switch(orientation){
        // left face
        case FACE_LEFT:
            // - fill -
            // TODO: put into separate function
            // for the entire height of the face
            for(y_pos=1;y_pos < settings->face_size;y_pos++){
                // draw a line, offset in y
                draw_diagonal_line(x,y+y_pos,-1,settings,renderer,settings->face_color+settings->light[0],face_id);
            }
            // draw bottom
            draw_diagonal_line(x,y,-1,settings,renderer,0,0);
            // draw top
            draw_diagonal_line(x,y+settings->face_size,-1,settings,renderer,0,0);
            // draw left edge
            draw_vertical_line(x,y,settings,renderer,0,0);
            // draw right edge
            draw_vertical_line(x-settings->face_size,y+settings->face_size/2,settings,renderer,0,0);
            break;
        // right face
        case FACE_RIGHT:
            // - fill -
            // for the entire height of the face
            for(y_pos=1;y_pos < settings->face_size;y_pos++){
                // draw a line, offset in y
                draw_diagonal_line(x,y+y_pos,1,settings,renderer,settings->face_color+settings->light[1],face_id);
            }
            // draw bottom
            draw_diagonal_line(x,y,1,settings,renderer,0,0);
            // draw top
            draw_diagonal_line(x,y+settings->face_size,1,settings,renderer,0,0);
            // draw left edge
            draw_vertical_line(x,y,settings,renderer,0,0);
            // draw right edge
            draw_vertical_line(x+settings->face_size,y+settings->face_size/2,settings,renderer,0,0);
            break;
        // top face
        case FACE_TOP:
            // - fill -
            // offset y (since we're drawing the top face)
            y_pos = settings->face_size;
            // for the entire width of the face
            while(x_pos<settings->face_size){
                // draw a line
                draw_diagonal_line(x+x_pos,y+y_pos,-1,settings,renderer,settings->face_color+settings->light[2],face_id);
                // offset in y
                y_pos++;
                // draw a line
                draw_diagonal_line(x+x_pos,y+y_pos,-1,settings,renderer,settings->face_color+settings->light[2],face_id);
                // offset in x
                x_pos++;
                // draw a second line (two steps in x for each step in y)
                draw_diagonal_line(x+x_pos,y+y_pos,-1,settings,renderer,settings->face_color+settings->light[2],face_id);
                // offset in x
                x_pos++;
            }
            // draw bottom left
            draw_diagonal_line(x,y+settings->face_size,1,settings,renderer,0,0);
            // draw bottom right
            draw_diagonal_line(x,y+settings->face_size,-1,settings,renderer,0,0);
            // draw top left
            draw_diagonal_line(x-settings->face_size,y+1.5*settings->face_size,1,settings,renderer,0,0);
            // draw top right
            draw_diagonal_line(x+settings->face_size,y+1.5*settings->face_size,-1,settings,renderer,0,0);
            break;
        default:
            break;
    }
}

void draw_diagonal_line(int x, int y, int direction, Settings *settings, Renderer *renderer, unsigned char color, unsigned int face_id){
    // current pos
    int pos = 0;
    // util we've drawn the entire line
    while(pos<(int)settings->face_size){
        // TODO: avoid multiplication?
        // draw first pixel
        renderer->pixels[3*(pos*direction+x+renderer->image_width*y)] = color;
        renderer->pixels[1+3*(pos*direction+x+renderer->image_width*y)] = color;
        renderer->pixels[2+3*(pos*direction+x+renderer->image_width*y)] = color;
        renderer->face_map[pos*direction+x+renderer->image_width*y] = face_id;
        // increase x
        pos++;
        // draw second pixel
        renderer->pixels[3*(pos*direction+x+renderer->image_width*y)] = color;
        renderer->pixels[1+3*(pos*direction+x+renderer->image_width*y)] = color;
        renderer->pixels[2+3*(pos*direction+x+renderer->image_width*y)] = color;
        renderer->face_map[pos*direction+x+renderer->image_width*y] = face_id;
        // increase x
        pos++;
        // increase y
        y++;
    }
}

void draw_vertical_line(int x, int y, Settings *settings, Renderer *renderer, unsigned char color, unsigned int face_id){
    int pos = 0;
    // until we've drawn the entire line
    while(pos<(int)settings->face_size){
        // draw pixel
        renderer->pixels[3*(x+renderer->image_width*(pos+y))] = color;
        renderer->pixels[1+3*(x+renderer->image_width*(pos+y))] = color;
        renderer->pixels[2+3*(x+renderer->image_width*(pos+y))] = color;
        renderer->face_map[x+renderer->image_width*(pos+y)] = face_id;
        // increase y
        pos++;
    }
}

void draw_number(Renderer *renderer, unsigned int x, unsigned int y, unsigned int number){
    // maximum number of digits to print
    const unsigned int MAX_DIGITS = 5;
    // iterators
    unsigned int row,pixel;
    unsigned int i;
    // offset relative to center of face
    int offset;
    // array of digits
    unsigned char digits[MAX_DIGITS];
    // initialize digits to 0 (to note that they should not be drawn)
    memset(digits,0,MAX_DIGITS*sizeof(char));
    // for each digit in number
    i=0;
    do{
        // get last digit
        digits[i] = number%10;
        // remove digit from number
        number /= 10;
        // increase iterator
        i++;
    }
    while(number != 0 && i!=MAX_DIGITS);
    // center digits
    offset = i*2-1;
    // for each digit
    while(i>0){
        // decrease iterator
        i--;
        // - draw digit -
        // for each row
        for(row=0;row<6;row++){
            // read correct byte
            unsigned char byte = font_bits[digits[i]/2+5*row];
            // if it's an odd digit
            if(digits[i]&1){
                // get upper nybble
                byte = byte >> 4;
            }
            // for each pixel
            for(pixel=0;pixel<4;pixel++)
            {
                // get correct bit
                unsigned char bit = byte &1;
                // shift bits in nybble one step
                byte = byte >> 1;
                // if the pixel is filled in
                if(bit)
                {
                    // add it to the image
                    renderer->pixels[(pixel+x-offset)*3+(4+y-row)*3*renderer->image_width] = 0;
                    renderer->pixels[1+(pixel+x-offset)*3+(4+y-row)*3*renderer->image_width] = 0;
                    renderer->pixels[2+(pixel+x-offset)*3+(4+y-row)*3*renderer->image_width] = 0;
                }
            }
        }
        offset-= 4;
    }
}
