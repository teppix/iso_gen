#include <iostream>
#include <string.h>
// used for writing the image file
#include "Magick++.h"

void draw_face(int x, int y, unsigned orientation, unsigned face_size, char *pixels, unsigned image_width, unsigned char color);

void draw_diagonal_line(int x, int y, int direction, unsigned face_size, char *pixels, unsigned image_width, char color);

void draw_vertical_line(int x, int y, unsigned face_size, char *pixels, unsigned image_width, char color);

int main();
