/* (C) 2020 Nicolaus Anderson */

/*
Functions for getting and setting pixels.
*/

#ifndef CUBR_IMAGE_H
#define CUBR_IMAGE_H

#include <Copper.h>
#include "cubr_base.h"

namespace cubr {

// Accepts an image and returns an object with members width,height.
Cu::ForeignFunc::Result
GetImageDimensions( Cu::FFIServices& );

// Accepts an image and returns an object with members red,green,blue,alpha.
Cu::ForeignFunc::Result
GetImagePixel( Cu::FFIServices& );

// Accepts an image, an x value, a y value, and a object with members red,green,blue,alpha to set a pixel color.
Cu::ForeignFunc::Result
SetImagePixel( Cu::FFIServices& );

} // cubr

#endif // CUBR_IMAGE_H
