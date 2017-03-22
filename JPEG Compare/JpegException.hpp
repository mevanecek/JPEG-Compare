//
//  JpegException.hpp
//  JPEG Compare
//
//  Created by Matt Vanecek on 3/21/17.
//  Copyright © 2017 Matthew Vanecek. All rights reserved.
//

#ifndef JpegException_hpp
#define JpegException_hpp

#include <stdio.h>
#include <exception>

class JpegException : public std::exception {
    const char * _message;
    JpegException(){}
public:
    JpegException(const char * msg) throw() : _message(msg) {}
    const char * what() const throw();
};

#endif /* JpegException_hpp */
