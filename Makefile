all:
	g++  -Wall -g -o app app.cpp -I/usr/include/ImageMagick -lMagick++ -lMagickCore
