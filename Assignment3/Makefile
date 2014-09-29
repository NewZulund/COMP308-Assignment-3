CC = g++
LPATH = -L/usr/pkg/lib
LDPATH = -Wl,-R/usr/pkg/lib 
CFLAGS=-g -Wall
LIBS=-lGL -lglut -lGLU -ljpeg -lm -lGLEW
IPATH= -I/usr/pkg/include

all: TextureDemo
TextureDemo : TextureDemo.o G308_ImageLoader.o G308_Geometry.o
	$(CC) -o TextureDemo TextureDemo.o G308_ImageLoader.o G308_Geometry.o $(LIBS) $(LPATH) $(LDPATH)
TextureDemo.o : TextureDemo.cpp
	$(CC) -c $(CFLAGS) TextureDemo.cpp $(IPATH)  
G308_ImageLoader.o :
	$(CC) -c $(CFLAGS) G308_ImageLoader.cpp $(IPATH)
G308_Geometry.o : G308_Geometry.cpp G308_Geometry.h 
	$(CC) -c $(CFLAGS) G308_Geometry.cpp $(IPATH)
clean :
	rm -rf *.o
	rm TextureDemo
