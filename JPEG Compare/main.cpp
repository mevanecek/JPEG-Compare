//
//  main.cpp
//  JPEG Compare
//
//  Created by Matt Vanecek on 3/20/17.
//  Copyright © 2017 Matthew Vanecek. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <setjmp.h>
#include "jpeglib.h"

FILE _iob[] = { *stdin, *stdout, *stderr };

#ifdef _WIN64
extern "C" FILE * __cdecl __iob_func(void)
{
	return _iob;
}
#endif

struct my_error_mgr {
    struct jpeg_error_mgr pub;    /* "public" fields */
    jmp_buf setjmp_buffer;        /* for return to caller */
};

typedef struct my_error_mgr *my_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 */

METHODDEF(void)
my_error_exit (j_common_ptr dcinfo) {
    /* dcinfo->err really points to a my_error_mgr struct, so coerce pointer */
    my_error_ptr myerr = (my_error_ptr) dcinfo->err;

    /* Always display the message. */
    /* We could postpone this until after returning, if we chose. */
    (*dcinfo->err->output_message) (dcinfo);

    /* Return control to the setjmp point */
    longjmp(myerr->setjmp_buffer, 1);
}


int main(int argc, const char * argv[]) {

#ifdef _WIN64
	const char * jpgFile = "C:\\Personal\\03196514\\Documents\\GitHub\\JPEG-Compare\\20160530_guitar_0018.jpg";
    const char * jpgOutFile = "C:\\Personal\\03196514\\Documents\\GitHub\\JPEG-Compare\\20160530_guitar_0018-new.jpg";
#else
    const char * jpgFile = "/Users/mattvanecek/Documents/XCode Projects/CPP Code Clinic/JPEG Compare/20160530_guitar_0018.jpg";
    const char * jpgOutFile = "/Users/mattvanecek/Documents/XCode Projects/CPP Code Clinic/JPEG Compare/20160530_guitar_0018-new.jpg";
#endif

    FILE* jpgFh;
    int row_stride;
    int jpg_size;

	struct jpeg_compress_struct cinfo;
	struct jpeg_decompress_struct dcinfo;
    struct my_error_mgr jerr;

	unsigned char * image;
	unsigned char * ptr;
	JSAMPARRAY row;
	JSAMPROW row_pointer[1];      /* pointer to JSAMPLE row[s] */

    if ((jpgFh = fopen(jpgFile, "rb")) == NULL) {
        fprintf(stderr, "can't open %s\n", jpgFile);
        return 0;
    }
    
    /* We set up the normal JPEG error routines, then override error_exit. */
    dcinfo.err = jpeg_std_error(&jerr.pub);
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;

    /* Establish the setjmp return context for my_error_exit to use. */
    if (setjmp(jerr.setjmp_buffer)) {
        /* If we get here, the JPEG code has signaled an error.
         * We need to clean up the JPEG object, close the input file, and return.
         */
        jpeg_destroy_decompress(&dcinfo);
        fclose(jpgFh);
        return 0;
    }

    /* Now we can initialize the JPEG decompression object. */
    jpeg_create_decompress(&dcinfo);
    jpeg_stdio_src(&dcinfo, jpgFh);
    (void) jpeg_read_header(&dcinfo, TRUE);
    jpeg_calc_output_dimensions(&dcinfo);

    /* JSAMPLEs per row in output buffer */
    row_stride = dcinfo.output_width * dcinfo.output_components;
    jpg_size = row_stride * dcinfo.output_height;

    printf("image_width    = %i\n", dcinfo.image_width);
    printf("image _height  = %i\n", dcinfo.image_height);
    printf("output_width   = %i\n", dcinfo.output_width);
    printf("output_height  = %i\n", dcinfo.output_height);
    printf("num components = %i\n", dcinfo.num_components);
    printf("color space    = %i\n", dcinfo.jpeg_color_space);
    printf("row stride     = %i\n", row_stride);
    printf("image size     = %i\n", jpg_size);

    (void) jpeg_start_decompress(&dcinfo);

    /* Make a one-row-high sample array that will go away when done with image */
	image = (unsigned char *)calloc(jpg_size, sizeof(unsigned char));
	//row = (dcinfo.mem->alloc_sarray)((j_common_ptr)&dcinfo, JPOOL_IMAGE, row_stride, 1);

    while (dcinfo.output_scanline < dcinfo.output_height) {
		ptr = image + (dcinfo.output_scanline * row_stride);
		// (void)jpeg_read_scanlines(&dcinfo, row, 1);
		(void)jpeg_read_scanlines(&dcinfo, &ptr, 1);
		//memcpy(ptr, row, row_stride);
	}

	fprintf(stderr, "Read %i rows\n", dcinfo.output_scanline);

    jpeg_destroy_decompress(&dcinfo);
    fclose(jpgFh);

	jpeg_create_compress(&cinfo);
	if ((jpgFh = fopen(jpgOutFile, "wb")) == NULL) {
		fprintf(stderr, "can't open %s\n", jpgOutFile);
		exit(1);
	}
	jpeg_stdio_dest(&cinfo, jpgFh);

	cinfo.image_width = dcinfo.output_width;        /* image width and height, in pixels */
	cinfo.image_height = dcinfo.output_height;
	cinfo.input_components = dcinfo.num_components; /* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB;					/* colorspace of input image */

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, 100, TRUE);
	jpeg_start_compress(&cinfo, TRUE);

	while (cinfo.next_scanline < cinfo.image_height) {
		ptr = image + (cinfo.next_scanline * row_stride);
		row_pointer[0] = ptr;
		(void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);

	fclose(jpgFh);
	jpeg_destroy_compress(&cinfo);

	free(image);
	return 0;
}
