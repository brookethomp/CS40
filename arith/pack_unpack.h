/*******************************************************************************
 *
 *                     pack_unpack.h
 *
 *     Assignment: arith
 *     Authors: Brooke Thompson (bthomp05) and Richard Geoghegan (rgeogh01)
 *     Date:     10/20/23
 *
 *     This file provides the interface of the pack_unpack class.
 *
 ******************************************************************************/
#ifndef PACK_UNPACK_INCLUDED
#define PACK_UNPACK_INCLUDED

#include <stdlib.h>
#include <math.h>

#include "a2methods.h"
#include "assert.h"
#include "bitpack.h"
#include "arith40.h"
#include "pnm_cv.h"

A2Methods_UArray2 cv_to_word(A2Methods_UArray2 image, 
                        A2Methods_mapfun *map, A2Methods_T methods);

A2Methods_UArray2 word_to_cv(A2Methods_UArray2 image, 
                        A2Methods_mapfun *map, A2Methods_T methods);

A2Methods_UArray2 word_to_bitpack(A2Methods_UArray2 image, 
                        A2Methods_mapfun *map, A2Methods_T methods);

A2Methods_UArray2 bitpack_to_word(A2Methods_UArray2 image,
                        A2Methods_mapfun *map, A2Methods_T methods);

#endif