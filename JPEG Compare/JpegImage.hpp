//
//  JpegImage.hpp
//  JPEG Compare
//
//  Created by Matt Vanecek on 3/21/17.
//  Copyright © 2017 Matthew Vanecek. All rights reserved.
//

#ifndef JpegImage_hpp
#define JpegImage_hpp

#include <stdio.h>
#include <string>
#include <jpeglib.h>
#include <setjmp.h>


class JpegImage {
    const std::string jpegFile;

    struct jpeg_decompress_struct dcinfo;
	struct jpeg_compress_struct cinfo;
	unsigned char * image;
    int row_stride;

	bool loaded; /* Has the file already been loaded */

    FILE * handle;

    // Private empty constuctor
	JpegImage() {};

	void freeBuffers();

public:
    JpegImage(const char * file) : jpegFile(file), image(nullptr) {};
    JpegImage(std::string & file) : jpegFile(file), image(nullptr) {};
    ~JpegImage();

    /**
      * @throws JpegException if the load was unsuccessful
      */
    void loadJpegFile();
	void writeJpegFile();
	void writeJpegFile(std::string outFile);
    
	void setImage(const unsigned char * image, const size_t row_stride, const size_t height);
	const unsigned char * getImage();
};

#endif /* JpegImage_hpp */
