all:
	gcc  -Wall -g -O2 -o app app.c settings.c renderer.c voxelgrid.c -lpng
