/*******************************************************************************
 *
 *                     read_print.h
 *
 *     Assignment: arith
 *     Authors: Brooke Thompson (bthomp05) and Richard Geoghegan (rgeogh01)
 *     Date:     10/20/23
 *
 *     This file provides the interface of the read_print class.
 *
 ******************************************************************************/
#ifndef READ_PRINT_INCLUDED
#define READ_PRINT_INCLUDED

#include <stdlib.h>
#include <stdbool.h>

#include "pnm.h"
#include "a2methods.h"
#include "assert.h"

Pnm_ppm read_uncompressed(FILE *fp, A2Methods_mapfun *map,
                        A2Methods_T methods);

A2Methods_UArray2 read_compressed(FILE *fp, A2Methods_mapfun *map,
                        A2Methods_T methods);

void print_compressed(A2Methods_UArray2 image, A2Methods_mapfun *map,
                        A2Methods_T methods);

#endif