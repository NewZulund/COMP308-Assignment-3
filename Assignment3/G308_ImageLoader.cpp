//---------------------------------------------------------------------------
//
// Copyright (c) 2012 Taehyun Rhee, Daniel Atkins
//
// This software is provided 'as-is' for assignment of COMP308
// in ECS, Victoria University of Wellington,
// without any express or implied warranty.
// In no event will the authors be held liable for any
// damages arising from the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
//----------------------------------------------------------------------------

#include "G308_ImageLoader.h"
#include "string.h"

/*
 * Magical JPEG loader. You probably shouldn't edit this.
 */
int loadTextureFromJPEG(char* filename, TextureInfo *info) {

	//Gonna need a file pointer.
	FILE *fd;

	//Init the structs required by libjpeg
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	//This will keep track of where we are in the file
	unsigned char * line;

	//Redirect stderr so things aren't messy
	cinfo.err = jpeg_std_error(&jerr);

	//Init the decompress struct
	jpeg_create_decompress(&cinfo);

	//Try to open the file. Better code would return an error value,
	//but I'm just making it exit instead.
	if (0 == (fd = fopen(filename, "rb"))) {
		printf("Error opening file");
		exit(1);
	}
	//Self evident.
	info->filename = filename;
	//Point libjpeg at the file
	jpeg_stdio_src(&cinfo, fd);
	//Read in the JPEG header
	jpeg_read_header(&cinfo, TRUE);
	//Used to keep track of offset in 1-D pixel array
	unsigned int size = cinfo.image_width;

	//Populate the texture-info struct.
	//If it's not GL_RGB, then you have a really strange JPEG.
	info->height = cinfo.image_height;
	info->width = cinfo.image_width;
	info->format = GL_RGB;
	//If your JPEG somehow has alpha, you're gonna have a bad time.
	info->hasAlpha = false;

	//Assign an array for pixels
	unsigned char* image = (unsigned char*) malloc(sizeof(char) * 3 * cinfo.image_height * cinfo.image_width);

	//Begin magic.
	jpeg_start_decompress(&cinfo);
	while (cinfo.output_scanline < cinfo.output_height) {
		line = image
				+ (3 * size) * cinfo.output_scanline;
		jpeg_read_scanlines(&cinfo, &line, 1);
	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	//Allocate an array for the pixels. Why two? You'll see.
	info->textureData = (unsigned char*) malloc(sizeof(char) * 3 * cinfo.image_height * cinfo.image_width);

	//Copy the pixels from image to textureData in reverse row order. Because GL.
	for(int y = info->height-1; y >= 0; y--){
		for(int x = 0; x < info->width*3; x++){
				info->textureData[(info->height-y-1)*(info->width*3)+x] = image[y*info->width*3+x];
		}
	}
	free(image);

	return 0;
}




