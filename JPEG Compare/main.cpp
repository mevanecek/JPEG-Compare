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
#include <jpeglib.h>

struct my_error_mgr {
    struct jpeg_error_mgr pub;    /* "public" fields */
    jmp_buf setjmp_buffer;        /* for return to caller */
};

typedef struct my_error_mgr *my_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 */

METHODDEF(void)
my_error_exit (j_common_ptr cinfo) {
    /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
    my_error_ptr myerr = (my_error_ptr) cinfo->err;

    /* Always display the message. */
    /* We could postpone this until after returning, if we chose. */
    (*cinfo->err->output_message) (cinfo);

    /* Return control to the setjmp point */
    longjmp(myerr->setjmp_buffer, 1);
}


int main(int argc, const char * argv[]) {

    const char * jpgFile = "/Users/mattvanecek/Documents/XCode Projects/CPP Code Clinic/Ex_Files_CC_CPP_02/Exercise Files/Chap02/Images-small/78771293a.jpg";
    FILE* source;
    int row_stride;

    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;
    JSAMPARRAY buffer;

    if ((source = fopen(jpgFile, "rb")) == NULL) {
        fprintf(stderr, "can't open %s\n", jpgFile);
        return 0;
    }
    
    /* We set up the normal JPEG error routines, then override error_exit. */
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    /* Establish the setjmp return context for my_error_exit to use. */
    if (setjmp(jerr.setjmp_buffer)) {
        /* If we get here, the JPEG code has signaled an error.
         * We need to clean up the JPEG object, close the input file, and return.
         */
        jpeg_destroy_decompress(&cinfo);
        fclose(source);
        return 0;
    }

    /* Now we can initialize the JPEG decompression object. */
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, source);
    (void) jpeg_read_header(&cinfo, TRUE);

    printf("output_width   = %i\n", cinfo.image_width);
    printf("output_height  = %i\n", cinfo.image_height);
    printf("num components = %i\n", cinfo.num_components);
    printf("color space    = %i\n", cinfo.jpeg_color_space);

    (void) jpeg_start_decompress(&cinfo);

    /* JSAMPLEs per row in output buffer */
    row_stride = cinfo.output_width * cinfo.output_components;
    printf("row stride    = %i\n", row_stride);

    /* Make a one-row-high sample array that will go away when done with image */
    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

    while (cinfo.output_scanline < cinfo.output_height) {
        (void) jpeg_read_scanlines(&cinfo, buffer, 1);

        /* Assume put_scanline_someplace wants a pointer and sample count. */
        //put_scanline_someplace(buffer[0], row_stride);
    }
    

    jpeg_destroy_decompress(&cinfo);
    fclose(source);
    return 0;
}
