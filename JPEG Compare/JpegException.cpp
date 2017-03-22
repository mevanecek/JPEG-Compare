//
//  JpegException.cpp
//  JPEG Compare
//
//  Created by Matt Vanecek on 3/21/17.
//  Copyright © 2017 Matthew Vanecek. All rights reserved.
//

#include "JpegException.hpp"

const char * JpegException::what() const throw() {
     return _message;
}
