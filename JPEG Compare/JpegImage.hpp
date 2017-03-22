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

    struct jpeg_decompress_struct cinfo;
    JSAMPARRAY buffer;
    int row_stride;
    FILE * handle;
    

    // Private empty constuctor
    JpegImage();

public:
    JpegImage(const char * file) : jpegFile(file) {};
    JpegImage(std::string & file) : jpegFile(file) {};
    ~JpegImage();

    /**
      * @throws JpegException if the load was unsuccessful
      */
    bool loadJpegFile();

    

    
};

#endif /* JpegImage_hpp */
