#include <string.h>
#include <stdlib.h>
#include <png.h>

#include "settings.h"
#include "voxelgrid.h"
#include "renderer.h"
#include "font.xbm"

int renderer_save(Renderer *renderer, Settings *settings){
    // iterator
    unsigned int i;
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
    // draw the number of each face
    draw_numbers(renderer, settings);
}

void draw_numbers(Renderer *renderer, Settings *settings){
    // iterators
    unsigned int x,y,face;
    // - store the min and max coordinates for each face -
    //
    // minimum x coordinate
    unsigned int *min_x = (unsigned int*)malloc(renderer->num_faces*sizeof(int));
    unsigned int *min_y = (unsigned int*)malloc(renderer->num_faces*sizeof(int));
    unsigned int *max_x = (unsigned int*)malloc(renderer->num_faces*sizeof(int));
    unsigned int *max_y = (unsigned int*)malloc(renderer->num_faces*sizeof(int));
    // keep track if we've found the face at all
    char *found = (char*)malloc(renderer->num_faces*sizeof(char));

    // initialize so that every value will be greater
    memset(min_y,renderer->image_height,renderer->num_faces*sizeof(int));
    memset(min_x,renderer->image_width,renderer->num_faces*sizeof(int));
    // initialize to zero
    memset(max_x,0,renderer->num_faces*sizeof(int));
    memset(max_y,0,renderer->num_faces*sizeof(int));

    // initialize to not found
    memset(found,0,renderer->num_faces*sizeof(char));

    // for each pixel
    for(x=0;x<renderer->image_width;x++){
        for(y=0;y<renderer->image_height;y++){
            // get the pixel value
            unsigned int face_id = renderer->face_map[x+renderer->image_width*y];
            // if it's not the background
            if(face_id != 0){
                // find which face it is
                face_id--;
                // update minimum
                min_x[face_id] = x<min_x[face_id]?x:min_x[face_id];
                min_y[face_id] = y<min_y[face_id]?y:min_y[face_id];
                // update maximum
                max_x[face_id] = x>max_x[face_id]?x:max_x[face_id];
                max_y[face_id] = y>max_y[face_id]?y:max_y[face_id];
                // tag face as found
                found[face_id] = 1;
            }
        }
    }
    // for each face
    for(face=0;face<renderer->num_faces;face++){
        // if we've found it
        if(found[face]){
            // draw the number in the middle of each face
            // middle x pos
            // TODO: use better algorithm? weight in all pixels
            unsigned middle_x = (min_x[face]+max_x[face])/2;
            // middle y pos
            unsigned middle_y = (min_y[face]+max_y[face])/2;
            draw_number(renderer, middle_x, middle_y,face);
        }
    }
    // free our arrays
    free(min_x);
    free(min_y);
    free(max_x);
    free(max_y);
    free(found);
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
    // iterators
    unsigned int row,pixel;
    unsigned int digit;
    // offset relative to center of face
    // TODO: center number
    int offset=-3;
    // we want to print one digit even if it's zero
    unsigned char first_run = 1;
    // until number is zero
    while(number != 0 || first_run){
        // no longer the first run
        first_run = 0;
        // get last digit
        digit = number%10;
        // remove digit from number
        number /= 10;
        // - draw digit -
        // for each row
        for(row=0;row<6;row++){
            // read correct byte
            unsigned char byte = font_bits[digit/2+5*row];
            // if it's an odd digit
            if(digit&1){
                // move down upper nybble
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
        offset+= 4;
    }
}
