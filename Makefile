all:
	g++  -Wall -g -o app app.cpp voxelgrid.c -I/usr/include/ImageMagick -lMagick++ -lMagickCore
