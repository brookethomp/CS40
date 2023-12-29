/*******************************************************************************
 *
 *                     rgb_cv_conversion.h
 *
 *     Assignment: arith
 *     Authors: Brooke Thompson (bthomp05) Richard Geoghegan (rgeogh01)
 *     Date:     10/20/23
 *
 *     This file provides the interface of the rgb_cv_conversion class.
 *
 ******************************************************************************/
#ifndef RGB_CV_CONVERSION_INCLUDED
#define RGB_CV_CONVERSION_INCLUDED

#include <stdlib.h>

#include "pnm.h"
#include "a2methods.h"
#include "assert.h"
#include "pnm_cv.h"

A2Methods_UArray2 rgb_to_cv(Pnm_ppm image, A2Methods_mapfun *map, 
                        A2Methods_T methods);

Pnm_ppm cv_to_rgb(A2Methods_UArray2 image, A2Methods_mapfun *map, 
                        A2Methods_T methods);
                        
#endif