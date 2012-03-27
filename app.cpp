#include "app.h"

// TODO: split to separate files

void draw_numbers(char *pixels,unsigned width, unsigned height, unsigned num_faces){
    // for each number
    for(unsigned i=0;i<num_faces;i++){
        // - get the minimum and maximum position of pixels -
        // minimum position
        unsigned min_x = 9001; // it's over 9000!
        unsigned min_y = 9001;
        // maximum position
        unsigned max_x = 0;
        unsigned max_y = 0;
        // for each pixel
            // - update minimum -
            // - update maximum -
        // TODO: draw numbers
    }
}

void draw_face(int x, int y, unsigned orientation, unsigned face_size, char *pixels, unsigned image_width, unsigned char color)
{
    // used for filling faces
    unsigned x_pos = 1;
    unsigned y_pos = 1;
    switch(orientation)
    {
        // left face
        case 0:
            // - fill -
            // for the entire height of the face
            for(y_pos=1;y_pos < face_size;y_pos++)
            {
                // draw a line, offset in y
                draw_diagonal_line(x,y+y_pos,-1,face_size,pixels,image_width,color);
            }
            // draw bottom
            draw_diagonal_line(x,y,-1,face_size,pixels,image_width,0);
            // draw top
            draw_diagonal_line(x,y+face_size,-1,face_size,pixels,image_width,0);
            // draw left edge
            draw_vertical_line(x,y,face_size,pixels,image_width,0);
            // draw right edge
            draw_vertical_line(x-face_size,y+face_size/2,face_size,pixels,image_width,0);
            break;
        // right face
        case 1:
            // - fill -
            // for the entire height of the face
            for(y_pos=1;y_pos < face_size;y_pos++)
            {
                // draw a line, offset in y
                draw_diagonal_line(x,y+y_pos,1,face_size,pixels,image_width,color);
            }
            // draw bottom
            draw_diagonal_line(x,y,1,face_size,pixels,image_width,0);
            // draw top
            draw_diagonal_line(x,y+face_size,1,face_size,pixels,image_width,0);
            // draw left edge
            draw_vertical_line(x,y,face_size,pixels,image_width,0);
            // draw right edge
            draw_vertical_line(x+face_size,y+face_size/2,face_size,pixels,image_width,0);
            break;
        // top face
        case 2:
            // - fill -
            // offset y (since we're drawing the top face)
            y_pos = face_size;
            // for the entire width of the face
            while(x_pos<face_size)
            {
                // draw a line
                draw_diagonal_line(x+x_pos,y+y_pos,-1,face_size,pixels,image_width,color);
                // offset in y
                y_pos++;
                // draw a line
                draw_diagonal_line(x+x_pos,y+y_pos,-1,face_size,pixels,image_width,color);
                // offset in x
                x_pos++;
                // draw a second line (two steps in x for each step in y)
                draw_diagonal_line(x+x_pos,y+y_pos,-1,face_size,pixels,image_width,color);
                // offset in x
                x_pos++;
            }
            // draw bottom left
            draw_diagonal_line(x,y+face_size,1,face_size,pixels,image_width,0);
            // draw bottom right
            draw_diagonal_line(x,y+face_size,-1,face_size,pixels,image_width,0);
            // draw top left
            draw_diagonal_line(x-face_size,y+1.5*face_size,1,face_size,pixels,image_width,0);
            // draw top right
            draw_diagonal_line(x+face_size,y+1.5*face_size,-1,face_size,pixels,image_width,0);
            break;
        default:
            break;
    }
}

void draw_diagonal_line(int x, int y, int direction, unsigned face_size, char *pixels, unsigned image_width, char color)
{
    // current pos
    int pos = 0;
    // util we've drawn the entire line
    while(pos<(int)face_size)
    {
        // draw first pixel
        pixels[3*(pos*direction+x+image_width*y)] = color;
        pixels[1+3*(pos*direction+x+image_width*y)] = color;
        pixels[2+3*(pos*direction+x+image_width*y)] = color;
        // increase x
        pos++;
        // draw second pixel
        pixels[3*(pos*direction+x+image_width*y)] = color;
        pixels[1+3*(pos*direction+x+image_width*y)] = color;
        pixels[2+3*(pos*direction+x+image_width*y)] = color;
        // increase x
        pos++;
        // increase y
        y++;
    }
}

void draw_vertical_line(int x, int y, unsigned face_size, char *pixels, unsigned image_width, char color)
{
    int pos = 0;
    // until we've drawn the entire line
    while(pos<(int)face_size)
    {
        // draw pixel
        pixels[3*(x+image_width*(pos+y))] = color;
        pixels[1+3*(x+image_width*(pos+y))] = color;
        pixels[2+3*(x+image_width*(pos+y))] = color;
        // increase y
        pos++;
    }
}

int main(){
    // voxel data describing our structure
    // TODO: read from blender ?
    const unsigned int x_num_voxels= 4;
    const unsigned int y_num_voxels= 2;
    const unsigned int z_num_voxels= 3;
    // total number of voxels
    const unsigned int num_voxels =x_num_voxels*y_num_voxels*z_num_voxels;
    bool voxels[x_num_voxels][y_num_voxels][z_num_voxels];
    // initialize all to false
    memset(voxels,0,num_voxels*sizeof(bool));
    // set initial data
    // TODO: beautify
    voxels[0][0][0] = true;
    voxels[1][0][0] = true;
    voxels[2][0][0] = true;
    voxels[3][0][0] = true;
    voxels[0][1][0] = true;
    voxels[1][1][0] = true;
    voxels[2][1][0] = true;
    voxels[3][1][0] = true;
    voxels[0][0][1] = true;
    voxels[1][0][1] = true;
    voxels[2][0][1] = true;
    voxels[3][0][1] = true;
    voxels[0][1][1] = true;
    voxels[1][1][1] = true;
    voxels[2][1][1] = true;
    voxels[3][1][1] = false;
    voxels[0][0][2] = true;
    voxels[1][0][2] = true;
    voxels[2][0][2] = true;
    voxels[3][0][2] = true;
    voxels[0][1][2] = false;
    voxels[1][1][2] = false;
    voxels[2][1][2] = true;
    voxels[3][1][2] = false;
    // face size (in screen space)
    unsigned int face_size = 32;
    // TODO: fix size
    // - each voxel will be (face_size+1)*2 pixels wide -
    // - and face_size*1.5+a pixels high -
    // final image height (padded a bit)
    const unsigned image_height = 800;
    // final image width (padded by one voxel)
    const unsigned image_width = 800;
    // pixels for final image
    char pixels[3*image_height*image_width];
    // set all pixels to black
    memset(pixels,255,3*image_height*image_width*sizeof(char));
    // == decompose into faces ==
    // id of the currently drawn face
    // start at 1 since the color of the outlines is 0
    // TODO: start at 1
    unsigned char face_id = 100;
    // for each voxel 
    for(unsigned x=0;x<x_num_voxels;x++){
        for(unsigned y=0;y<y_num_voxels;y++){
            for(unsigned z=0;z<z_num_voxels;z++){
                // if the current voxel is filled
                if(voxels[x][y][z]){
                    // placement relative to other voxels
                    int offset_x = y-x;
                    int pos_x = 400+(offset_x)*(face_size);
                    int offset_y = -x-y+2*z;
                    int pos_y = 400+(offset_y)*(face_size/2);
                    // -- left face --
                    // - check visibility (to avoid increasing the id for faces not visible) -
                    // should we draw the face?
                    bool face_visible = true;
                    // unless we're on the far left side
                    if(x<x_num_voxels-1)
                    {
                        // check if there's a voxel directly in front of the left face
                        if(voxels[x+1][y][z])
                        {
                            // don't draw the face (will not be visible anyway)
                            face_visible=false;
                        }
                    }
                    // if there's nothing blocking
                    if(face_visible){
                        // draw left face
                        draw_face(pos_x,pos_y,0,face_size,pixels,image_width, face_id);
                        // increase face id
                        face_id++;
                    }
                    // -- right face --
                    // - check visibility (to avoid increasing the id for faces not visible) -
                    face_visible = true;
                    if(y<y_num_voxels-1)
                    {
                        // check if there's a voxel directly in front of the left face
                        if(voxels[x][y+1][z])
                        {
                            // don't draw the face (will not be visible anyway)
                            face_visible=false;
                        }
                    }
                    // if there's nothing blocking
                    if(face_visible){
                        // draw right face
                        draw_face(pos_x,pos_y,1,face_size,pixels,image_width, face_id);
                        // increase face id
                        face_id++;
                    }
                    // -- top face --
                    // - check visibility (to avoid increasing the id for faces not visible) -
                    face_visible = true;
                    if(z<z_num_voxels-1)
                    {
                        // check if there's a voxel directly in front of the left face
                        if(voxels[x][y][z+1])
                        {
                            // don't draw the face (will not be visible anyway)
                            face_visible=false;
                        }
                    }
                    // if there's nothing blocking
                    if(face_visible){
                        // draw top face
                        draw_face(pos_x,pos_y,2,face_size,pixels,image_width, face_id);
                        // increase face id
                        face_id++;
                    }
                }
            }
        }
    }
    // == write file  ==
    // create image from pixels
    Magick::Image image;
    image.read(image_width, image_height, "RGB", Magick::CharPixel, pixels);
    // flip image (since ImageMagick works upside down ;)
    image.flip();
    // write file to disk
    image.write("out.png");
    // return success
    return 0;
}
