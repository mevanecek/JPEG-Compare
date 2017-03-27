//
//  JpegImage.cpp
//  JPEG Compare
//
//  Created by Matt Vanecek on 3/21/17.
//  Copyright © 2017 Matthew Vanecek. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <string>

#include "JpegImage.hpp"
#include "JpegException.hpp"

METHODDEF(void) error_exit (j_common_ptr dcinfo) {
    /* dcinfo->err really points to a my_error_mgr struct, so coerce pointer */
    JpegImageErrorMgrPtr myerr = (JpegImageErrorMgrPtr) dcinfo->err;

    /* Always display the message. */
    /* We could postpone this until after returning, if we chose. */
    (*dcinfo->err->output_message) (dcinfo);

    /* Return control to the setjmp point */
    longjmp(myerr->setjmp_buffer, 1);
}

/**
  * Constructors
  */
JpegImage::JpegImage(const char * file) : jpegFile(file), image(nullptr) {
    initialize();
}

JpegImage::JpegImage(std::string & file) : jpegFile(file), image(nullptr) {
    initialize();
}

/**
 * Destructor
 */
JpegImage::~JpegImage() {
    freeBuffers();
}

void JpegImage::freeBuffers() {
    if (image) {
        free(image);
        image = nullptr;
    }
}

void JpegImage::initializeJpegError() {
    jpegError.pub.error_exit = error_exit;
    dcinfo.err = jpeg_std_error(&jpegError.pub);
    cinfo.err = jpeg_std_error(&jpegError.pub);
}

void JpegImage::initialize() {
    initializeJpegError();
    height = 0;
    width = 0;
    numComponents = 0;
    row_stride = 0;

}

void JpegImage::initializeDecompress() {
}

void JpegImage::initializeCompress() {
}


/**
 * Read the JPEG data from the specified file and decompress it into the
 * image buffer.
 */
void JpegImage::loadJpegFile() {

	jpg_size = 0;
	unsigned char * ptr = nullptr;
    errno = 0;

    if ((handle = fopen(jpegFile.c_str(), "rb")) == NULL) {
        std::string errMsg(strerror(errno));
        errMsg = "Error opening " + jpegFile + ": " + errMsg;
        throw JpegException(errMsg.c_str());
    }
    
    /* Establish the setjmp return context for error manager to use. */
    if (setjmp(jpegError.setjmp_buffer)) {
        std::string errMsg(strerror(errno));
        errMsg = "A JPEG error occured for " + jpegFile + ": " + errMsg;
        jpeg_destroy_decompress(&dcinfo);
        fclose(handle);

        throw JpegException(errMsg.c_str());
    }

	/* Now we can initialize the JPEG decompression object. */
	jpeg_create_decompress(&dcinfo);
	jpeg_stdio_src(&dcinfo, handle);
	(void)jpeg_read_header(&dcinfo, TRUE);
	jpeg_calc_output_dimensions(&dcinfo);
    (void)jpeg_start_decompress(&dcinfo);

	/* JSAMPLEs per row in output buffer */
	row_stride = dcinfo.output_width * dcinfo.output_components;
	jpg_size = row_stride * dcinfo.output_height;
    height = dcinfo.output_height;
    width = dcinfo.output_width;
    numComponents = dcinfo.num_components;
    jpegColorSpace = dcinfo.jpeg_color_space;

    printf("image_width    = %i\n", dcinfo.image_width);
    printf("image _height  = %i\n", dcinfo.image_height);
    printf("output_width   = %i\n", dcinfo.output_width);
    printf("output_height  = %i\n", dcinfo.output_height);
    printf("num components = %i\n", dcinfo.num_components);
    printf("color space    = %i\n", dcinfo.jpeg_color_space);
    printf("row stride     = %i\n", row_stride);
    printf("image size     = %zu\n", jpg_size);

    image = (unsigned char *)calloc(jpg_size, sizeof(unsigned char));

	while (dcinfo.output_scanline < dcinfo.output_height) {
		ptr = image + (dcinfo.output_scanline * row_stride);
		(void)jpeg_read_scanlines(&dcinfo, &ptr, 1);
	}

	fprintf(stderr, "Read %i rows\n", dcinfo.output_scanline);

	jpeg_destroy_decompress(&dcinfo);
	fclose(handle);
    handle = nullptr;
	loaded = true;
}

void JpegImage::writeJpegFile() {
    writeJpegFile(jpegFile);
}

void JpegImage::writeJpegFile(std::string outFile) {
    unsigned char * ptr = nullptr;
    JSAMPROW row_pointer[1];

    jpeg_create_compress(&cinfo);

    if ((handle = fopen(outFile.c_str(), "wb")) == NULL) {
        fprintf(stderr, "can't open %s\n", outFile.c_str());
        exit(1);
    }
    jpeg_stdio_dest(&cinfo, handle);


    cinfo.image_width = getWidth();              /* image width and height, in pixels */
    cinfo.image_height = getHeight();
    cinfo.input_components = getNumComponents(); /* # of color components per pixel */
    cinfo.in_color_space = JCS_RGB;              /* colorspace of input image */

    printf("image_width    = %i\n", cinfo.image_width);
    printf("image _height  = %i\n", cinfo.image_height);
    printf("num components = %i\n", cinfo.input_components);

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 100, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    while (cinfo.next_scanline < cinfo.image_height) {
        ptr = (unsigned char *)(image + (cinfo.next_scanline * row_stride));
        row_pointer[0] = ptr;
        (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(handle);
    handle = nullptr;
}

/**
 * Need to be sure we have all the relevant measurements of the image,
 * and not just the image data
 */
void JpegImage::setImage(const unsigned char * imgData, const JDIMENSION imgHt,
                         const JDIMENSION imgWdt, const int imgNumComp) {
    if (imgHt == 0 || imgWdt == 0 || imgNumComp == 0) {
        fprintf(stderr, "Dimensions are invalid: H=%i, W=%i, NC=%i\n", imgHt, imgWdt, imgNumComp);
        return;
    }

    height = imgHt;
    width = imgWdt;
    numComponents = imgNumComp;
    row_stride = width * numComponents;
    jpg_size = row_stride * height;

    image = (unsigned char *)calloc(jpg_size, sizeof(unsigned char));
    memcpy(image, imgData, jpg_size);

}

const unsigned char * JpegImage::getImage() {
	return image;
}

const JDIMENSION JpegImage::getHeight() {
    JDIMENSION d = 0;
    if (loaded) {
        d = dcinfo.output_height;
    }
    else {
        d = height;
    }
    return d;
}

const JDIMENSION JpegImage::getWidth() {
    JDIMENSION d = 0;
    if (loaded) {
        d = dcinfo.output_width;
    }
    else {
        d = width;
    }
    return d;
}

const int JpegImage::getNumComponents() {
    JDIMENSION d = 0;
    if (loaded) {
        d = dcinfo.output_components;
    }
    else {
        d = numComponents;
    }
    return d;
}
