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


struct JpegImageErrorMgr {
    struct jpeg_error_mgr pub;    /* "public" fields */
    jmp_buf setjmp_buffer;        /* for return to caller */
};

typedef struct JpegImageErrorMgr *JpegImageErrorMgrPtr;

class JpegImage {
    const std::string jpegFile;

    struct jpeg_decompress_struct dcinfo;
	struct jpeg_compress_struct cinfo;
    struct JpegImageErrorMgr jpegError;

    unsigned char * image;
    JDIMENSION height;
    JDIMENSION width;
    J_COLOR_SPACE jpegColorSpace;

    int numComponents;
    int row_stride;
    size_t jpg_size;
    
	bool loaded; /* Has the file already been loaded */

    FILE * handle;

    // Private empty constuctor
	JpegImage() {};

    void initialize();
    void initializeJpegError();
    void initializeDecompress();
    void initializeCompress();
	void freeBuffers();

public:
    JpegImage(const char * file);
    JpegImage(std::string & file);
    ~JpegImage();

    /**
      * @throws JpegException if the load was unsuccessful
      */
    void loadJpegFile();
	void writeJpegFile();
	void writeJpegFile(std::string outFile);
    
    void setImage(const unsigned char * imgData, const JDIMENSION imgHt,
                  const JDIMENSION imgWdt, const int imgNumComp);
	const unsigned char * getImage();

    const JDIMENSION getHeight();
    const JDIMENSION getWidth();
    const int getNumComponents();

};

#endif /* JpegImage_hpp */
