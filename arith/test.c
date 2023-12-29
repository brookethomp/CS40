/*******************************************************************************
 *
 *                     test.c
 *
 *     Assignment: arith
 *     Authors: Brooke Thompson (bthomp05) and Richard ()
 *     Date:     10/20/23
 *
 *     This file provides a program that 
 *
 ******************************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "rgb_cv_conversion.h"
#include "pack_unpack.h"
#include "read_print.h"
#include "a2plain.h"
// #include "a2blocked.h"

unsigned int int_to_int(unsigned int k);

int main(int argc, char *argv[]) {

        FILE *fp;
        fp = fopen(argv[1], "r");
        
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods);
        
        /* default to best map */
        A2Methods_mapfun *map = methods->map_default; 
        
        //Pnm_ppm image = Pnm_ppmread(fp, methods);

        A2Methods_UArray2 compressed = read_compressed(fp, map, methods);
        A2Methods_UArray2 unpacked = bitpack_to_word(compressed, map, methods);
        A2Methods_UArray2 componentVid = word_to_cv(unpacked, map, methods);
        Pnm_ppm decompressed = cv_to_rgb(componentVid, map, methods);
        Pnm_ppmwrite(stdout, decompressed);
        // Pnm_ppm image = read_uncompressed(fp, map, methods);

        // A2Methods_UArray2 converted = rgb_to_cv(image, map, methods);

        // A2Methods_UArray2 words = cv_to_word(converted, map, methods);

        // A2Methods_UArray2 bitpacked = word_to_bitpack(words, map, methods);

        // print_compressed(bitpacked, map, methods);
        //A2Methods_UArray2 unpacked = bitpack_to_word(bitpacked, map, methods);

        //A2Methods_UArray2 componentVid = word_to_cv(unpacked, map, methods);



        //Pnm_ppm decompressed = cv_to_rgb(componentVid, map, methods);
        //(void) decompressed;
        //Pnm_ppmwrite(stdout, decompressed);
        //methods->free(&words);
        //methods->free(&converted);
        //methods->free(&componentVid);
        //Pnm_ppmfree(&image);
        //Pnm_ppmfree(&decompressed);
        fclose(fp);
        //(void) converted;
        (void) argc;
        return 0;
}