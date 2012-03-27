#ifndef APP_H
#define APP_H

#include <iostream>
#include <string.h>
// used for writing the image file
#include "Magick++.h"

#include "data.h"
#include "settings.h"

// the different face directions
enum FaceOrientation{
    FACE_LEFT,
    FACE_RIGHT,
    FACE_TOP
};

void draw_numbers(Data &data, Settings &settings);
void draw_face(int x, int y, FaceOrientation orientation, unsigned char color, Settings &settings, Data &data);
void draw_diagonal_line(int x, int y, int direction, Settings &settings, Data &data, unsigned char color);
void draw_vertical_line(int x, int y, Settings &settings, Data &data, unsigned char color);
int main();

#endif
