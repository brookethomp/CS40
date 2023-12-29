/*******************************************************************************
 *
 *                     compress40.c
 *
 *     Assignment: arith
 *     Authors: Brooke Thompson (bthomp05) and Richard Geoghegan (rgeogh01)
 *     Date:     10/20/23
 *
 *     This file provides a program that reads a ppm and writes a compressed 
 *     image and reads a compressed image and write a ppm.
 *
 ******************************************************************************/
#include "compress40.h"
#include "rgb_cv_conversion.h"
#include "pack_unpack.h"
#include "read_print.h"
#include "a2plain.h"

/********** compress40 ********
 *
 * Reads in a PPM and writes a compressed image.
 *
 * Parameters:
 *      FILE *input: Takes in a pointer to a PPM file to read in
 *
 * Return: N/A void function
 *
 * Expects
 *      Methods to not be null
 *      map to not be null
 * Notes:
 *      TBD
 ************************/
extern void compress40  (FILE *input) {
        A2Methods_T methods = uarray2_methods_plain;
        assert(methods);

        A2Methods_mapfun *map = methods->map_default;
        assert(map);

        Pnm_ppm image = read_uncompressed(input, map, methods);

        A2Methods_UArray2 cv = rgb_to_cv(image, map, methods);
        A2Methods_UArray2 words = cv_to_word(cv, map, methods);
        A2Methods_UArray2 bitpacked = word_to_bitpack(words, map, methods);
        print_compressed(bitpacked, map, methods);

        methods->free(&cv);
        methods->free(&words);
        methods->free(&bitpacked);
        Pnm_ppmfree(&image);
}

 /********** decompress40 ********
 *
 * Reads a compressed image, and writes PPM.
 *
 * Parameters:
 *      FILE *input: Takes in a pointer to a compressed image to read.
 *
 * Return: N/A void function
 *
 * Expects
 *      Methods to not be null
 *      map to not be null
 * Notes:
 *      TBD
 ************************/
extern void decompress40(FILE *input) {
        A2Methods_T methods = uarray2_methods_plain;  
        assert(methods);

        A2Methods_mapfun *map = methods->map_default;
        assert(map);

        A2Methods_UArray2 compressed = read_compressed(input, map, methods);
        A2Methods_UArray2 words = bitpack_to_word(compressed, map, methods);
        A2Methods_UArray2 cv = word_to_cv(words, map, methods);
        Pnm_ppm uncompressed = cv_to_rgb(cv, map, methods);

        methods->free(&compressed);
        methods->free(&words);
        methods->free(&cv);

        Pnm_ppmwrite(stdout, uncompressed);
        Pnm_ppmfree(&uncompressed);
}