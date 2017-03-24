//
//  JpegImage.cpp
//  JPEG Compare
//
//  Created by Matt Vanecek on 3/21/17.
//  Copyright © 2017 Matthew Vanecek. All rights reserved.
//

#include "JpegImage.hpp"

void JpegImage::freeBuffers() {
}

JpegImage::~JpegImage() {
	freeBuffers();
}

/**
 * Read the JPEG data from the specified file and decompress it into the
 * image buffer.
 */
void JpegImage::loadJpegFile() {

	size_t jpg_size = 0;
	unsigned char * ptr = nullptr;

	if ((handle = fopen(jpegFile.c_str(), "rb")) == NULL) {
		fprintf(stderr, "can't open %s\n", jpegFile.c_str());
	}

	/* Now we can initialize the JPEG decompression object. */
	jpeg_create_decompress(&dcinfo);
	jpeg_stdio_src(&dcinfo, handle);
	(void)jpeg_read_header(&dcinfo, TRUE);
	jpeg_calc_output_dimensions(&dcinfo);

	/* JSAMPLEs per row in output buffer */
	row_stride = dcinfo.output_width * dcinfo.output_components;
	jpg_size = row_stride * dcinfo.output_height;

	(void)jpeg_start_decompress(&dcinfo);

	image = (unsigned char *)calloc(jpg_size, sizeof(unsigned char));

	while (dcinfo.output_scanline < dcinfo.output_height) {
		ptr = image + (dcinfo.output_scanline * row_stride);
		(void)jpeg_read_scanlines(&dcinfo, &ptr, 1);
	}

	fprintf(stderr, "Read %i rows\n", dcinfo.output_scanline);

	jpeg_destroy_decompress(&dcinfo);
	fclose(handle);

	loaded = true;
}

void JpegImage::writeJpegFile() {
}

void JpegImage::writeJpegFile(std::string outFile) {
}

void JpegImage::setImage(const unsigned char * image, const size_t row_stride, const size_t height) {
}

const unsigned char * JpegImage::getImage() {
	return nullptr;
}
