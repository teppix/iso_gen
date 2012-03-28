#include "voxelgrid.h"
#include "app.h"


void draw_numbers(Data &data, Settings &settings){
    // - store the min and max coordinates for each face -
    //
    // minimum x coordinate
    unsigned int *min_x = new unsigned int[data.num_faces];
    // initialize so that every value will be greater
    memset(min_x,data.image_width,data.num_faces*sizeof(int));
    // minimum y coordinate
    unsigned int *min_y = new unsigned int[data.num_faces];
    // initialize so that every value will be greater
    memset(min_y,data.image_height,data.num_faces*sizeof(int));
    // maximum x coordinate
    unsigned int *max_x = new unsigned int[data.num_faces];
    // initialize to zero
    memset(max_x,0,data.num_faces*sizeof(int));
    // maximum y coordinate
    unsigned int *max_y = new unsigned int[data.num_faces];
    // initialize to zero
    memset(max_y,0,data.num_faces*sizeof(int));

    // keep track if we've found the face at all
    bool *found = new bool[data.num_faces];
    memset(found,0,data.num_faces*sizeof(bool));

    // for each pixel
    for(unsigned x=0;x<data.image_width;x++){
        for(unsigned y=0;y<data.image_height;y++){
            // get the pixel value
            unsigned char pixel_value = data.pixels[x*3+data.image_width*y*3];
            // if it's not the background or the border
            if(pixel_value != 255 && pixel_value != 0){
                // find which face it is
                unsigned char face = pixel_value - settings.face_color;
                // update minimum
                min_x[face] = x<min_x[face]?x:min_x[face];
                min_y[face] = y<min_y[face]?y:min_y[face];
                // update maximum
                max_x[face] = x>max_x[face]?x:max_x[face];
                max_y[face] = y>max_y[face]?y:max_y[face];
                // tag face as found
                found[face] = true;
            }
        }
    }
    // for each face
    for(unsigned face=0;face<data.num_faces;face++){
        // if we've found it
        if(found[face]){
            // TODO: draw numbers
            // TODO: remove debug
            // for now, draw a dot in the middle of each face
            // middle x pos
            unsigned middle_x = (min_x[face]+max_x[face])/2;
            // middle y pos
            unsigned middle_y = (min_y[face]+max_y[face])/2;
            data.pixels[middle_x*3+middle_y*3*data.image_width] = 0;
            data.pixels[1+middle_x*3+middle_y*3*data.image_width] = 0;
            data.pixels[2+middle_x*3+middle_y*3*data.image_width] = 0;
        }
    }
    // free our arrays
    delete[] min_x;
    delete[] min_y;
    delete[] max_x;
    delete[] max_y;
}

void draw_face(int x, int y, FaceOrientation orientation, unsigned char color, Settings &settings, Data &data){
    // brighten color with base color
    color += settings.face_color;
    // used for filling faces
    unsigned x_pos = 1;
    unsigned y_pos = 1;
    switch(orientation){
        // left face
        case FACE_LEFT:
            // - fill -
            // for the entire height of the face
            for(y_pos=1;y_pos < settings.face_size;y_pos++){
                // draw a line, offset in y
                draw_diagonal_line(x,y+y_pos,-1,settings,data,color);
            }
            // draw bottom
            draw_diagonal_line(x,y,-1,settings,data,0);
            // draw top
            draw_diagonal_line(x,y+settings.face_size,-1,settings,data,0);
            // draw left edge
            draw_vertical_line(x,y,settings,data,0);
            // draw right edge
            draw_vertical_line(x-settings.face_size,y+settings.face_size/2,settings,data,0);
            break;
        // right face
        case FACE_RIGHT:
            // - fill -
            // for the entire height of the face
            for(y_pos=1;y_pos < settings.face_size;y_pos++){
                // draw a line, offset in y
                draw_diagonal_line(x,y+y_pos,1,settings,data,color);
            }
            // draw bottom
            draw_diagonal_line(x,y,1,settings,data,0);
            // draw top
            draw_diagonal_line(x,y+settings.face_size,1,settings,data,0);
            // draw left edge
            draw_vertical_line(x,y,settings,data,0);
            // draw right edge
            draw_vertical_line(x+settings.face_size,y+settings.face_size/2,settings,data,0);
            break;
        // top face
        case FACE_TOP:
            // - fill -
            // offset y (since we're drawing the top face)
            y_pos = settings.face_size;
            // for the entire width of the face
            while(x_pos<settings.face_size){
                // draw a line
                draw_diagonal_line(x+x_pos,y+y_pos,-1,settings,data,color);
                // offset in y
                y_pos++;
                // draw a line
                draw_diagonal_line(x+x_pos,y+y_pos,-1,settings,data,color);
                // offset in x
                x_pos++;
                // draw a second line (two steps in x for each step in y)
                draw_diagonal_line(x+x_pos,y+y_pos,-1,settings,data,color);
                // offset in x
                x_pos++;
            }
            // draw bottom left
            draw_diagonal_line(x,y+settings.face_size,1,settings,data,0);
            // draw bottom right
            draw_diagonal_line(x,y+settings.face_size,-1,settings,data,0);
            // draw top left
            draw_diagonal_line(x-settings.face_size,y+1.5*settings.face_size,1,settings,data,0);
            // draw top right
            draw_diagonal_line(x+settings.face_size,y+1.5*settings.face_size,-1,settings,data,0);
            break;
        default:
            break;
    }
}

void draw_diagonal_line(int x, int y, int direction, Settings &settings, Data &data, unsigned char color){
    // current pos
    int pos = 0;
    // util we've drawn the entire line
    while(pos<(int)settings.face_size){
        // draw first pixel
        data.pixels[3*(pos*direction+x+data.image_width*y)] = color;
        data.pixels[1+3*(pos*direction+x+data.image_width*y)] = color;
        data.pixels[2+3*(pos*direction+x+data.image_width*y)] = color;
        // increase x
        pos++;
        // draw second pixel
        data.pixels[3*(pos*direction+x+data.image_width*y)] = color;
        data.pixels[1+3*(pos*direction+x+data.image_width*y)] = color;
        data.pixels[2+3*(pos*direction+x+data.image_width*y)] = color;
        // increase x
        pos++;
        // increase y
        y++;
    }
}

void draw_vertical_line(int x, int y, Settings &settings, Data &data, unsigned char color){
    int pos = 0;
    // until we've drawn the entire line
    while(pos<(int)settings.face_size){
        // draw pixel
        data.pixels[3*(x+data.image_width*(pos+y))] = color;
        data.pixels[1+3*(x+data.image_width*(pos+y))] = color;
        data.pixels[2+3*(x+data.image_width*(pos+y))] = color;
        // increase y
        pos++;
    }
}

int main(){
    // load voxel data from file
    VoxelGrid *voxelgrid = voxelgrid_load ("in.vox");

    // calculated data used by several of the functions
    Data data;
    // = get user settings =
    // TODO: get input from user instead
    Settings settings;
    // color of the faces
    settings.face_color = 100;
    // face size (in screen space)
    settings.face_size = 32;
    // TODO: fix size
    // - each voxel will be (face_size+1)*2 pixels wide -
    // - and face_size*1.5+a pixels high -
    // final image height (padded by one voxel)
    data.image_height = (voxelgrid->dim_x+voxelgrid->dim_y+2*voxelgrid->dim_z+1)*settings.face_size/2;
    // final image width (padded by one voxel)
    data.image_width = settings.face_size*(voxelgrid->dim_x+voxelgrid->dim_y+1);
    // where to place the 0,0,0 voxel
    unsigned int origin_x = voxelgrid->dim_x*settings.face_size + settings.face_size/2;
    unsigned int origin_y = (voxelgrid->dim_x+voxelgrid->dim_y-2)*settings.face_size/2+ settings.face_size/4;
    // pixels for final image
    data.pixels = new unsigned char[3*data.image_height*data.image_width];
    // set all pixels to black
    memset(data.pixels,255,3*data.image_height*data.image_width*sizeof(char));
    // == decompose into faces ==
    // id of the currently drawn face
    unsigned char face_id = 0;
    // for each voxel 
    for(unsigned x=0;x<voxelgrid->dim_x;x++){
        for(unsigned y=0;y<voxelgrid->dim_y;y++){
            for(unsigned z=0;z<voxelgrid->dim_z;z++){
                // if the current voxel is filled
                if (VOXEL (voxelgrid, x, y, z) == 1) {
                    // placement relative to other voxels
                    int offset_x = y-x;
                    int pos_x = origin_x+(offset_x)*(settings.face_size);
                    int offset_y = -x-y+2*z;
                    int pos_y = origin_y+(offset_y)*(settings.face_size/2);
                    // -- left face --
                    // - check visibility (to avoid increasing the id for faces not visible) -
                    // should we draw the face?
                    bool face_visible = true;
                    // unless we're on the far left side
                    if(x<voxelgrid->dim_x-1) {
                        // check if there's a voxel directly in front of the left face
                        if(VOXEL(voxelgrid, x+1, y, z)==1){
                            // don't draw the face (will not be visible anyway)
                            face_visible=false;
                        }
                    }
                    // if there's nothing blocking
                    if(face_visible){
                        // draw left face
                        draw_face(pos_x,pos_y,FACE_LEFT,face_id, settings, data);
                        // increase face id
                        face_id++;
                    }
                    // -- right face --
                    // - check visibility (to avoid increasing the id for faces not visible) -
                    face_visible = true;
                    if(y<voxelgrid->dim_y-1) {
                        // check if there's a voxel directly in front of the left face
                        if(VOXEL(voxelgrid, x, y+1, z)==1) {
                            // don't draw the face (will not be visible anyway)
                            face_visible=false;
                        }
                    }
                    // if there's nothing blocking
                    if(face_visible){
                        // draw right face
                        draw_face(pos_x,pos_y,FACE_RIGHT,face_id, settings, data);
                        // increase face id
                        face_id++;
                    }
                    // -- top face --
                    // - check visibility (to avoid increasing the id for faces not visible) -
                    face_visible = true;
                    if(z<voxelgrid->dim_z-1)
                    {
                        // check if there's a voxel directly in front of the left face
                        if(VOXEL(voxelgrid, x, y, z+1)==1)
                        {
                            // don't draw the face (will not be visible anyway)
                            face_visible=false;
                        }
                    }
                    // if there's nothing blocking
                    if(face_visible){
                        // draw top face
                        draw_face(pos_x,pos_y,FACE_TOP,face_id, settings, data);
                        // increase face id
                        face_id++;
                    }
                }
            }
        }
    }
    // set number of faces
    data.num_faces = face_id;
    // draw the number of each face
    draw_numbers(data, settings);
    // == write file  ==
    // create image from pixels
    Magick::Image image;
    image.read(data.image_width, data.image_height, "RGB", Magick::CharPixel, data.pixels);
    // flip image (since ImageMagick works upside down ;)
    image.flip();
    // write file to disk
    image.write("out.png");

    // free pixels
    delete [] data.pixels;

    voxelgrid_free(voxelgrid);
    // return success
    return 0;
}
