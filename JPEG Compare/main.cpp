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
#include "JpegImage.hpp"
#include "JpegException.hpp"

#ifdef _WIN64
FILE _iob[] = { *stdin, *stdout, *stderr };

extern "C" FILE * __cdecl __iob_func(void)
{
	return _iob;
}
#endif


int main(int argc, const char * argv[]) {

#ifdef _WIN64
	const char * jpgFile = "C:\\Personal\\03196514\\Documents\\GitHub\\JPEG-Compare\\20160530_guitar_0018.jpg";
    const char * jpgOutFile = "C:\\Personal\\03196514\\Documents\\GitHub\\JPEG-Compare\\20160530_guitar_0018-new.jpg";
#else
    const char * jpgFile = "/Users/mattvanecek/Documents/XCode Projects/CPP Code Clinic/JPEG Compare/20160530_guitar_0018.jpg";
    const char * jpgOutFile = "/Users/mattvanecek/Documents/XCode Projects/CPP Code Clinic/JPEG Compare/20160530_guitar_0018-new.jpg";
#endif

    try {
        JpegImage decomp(jpgFile);
        decomp.loadJpegFile();
        // decomp.writeJpegFile(jpgOutFile);

        JpegImage comp(jpgOutFile);
        comp.setImage(decomp.getImage(), decomp.getHeight(), decomp.getWidth(), decomp.getNumComponents());
        comp.writeJpegFile();
        
    }
    catch (JpegException ex) {
        fprintf(stderr, "Error processing the JPG file:\n%s", ex.what());
    }
    return 0;
}
