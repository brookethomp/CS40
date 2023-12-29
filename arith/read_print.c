/******************************************************************************
 *
 *                     read_print.c
 *
 *     Assignment: arith
 *     Authors: Brooke Thompson (bthomp05) and Richard Geoghegan (rgeogh01)
 *     Date:     10/20/23
 *
 *     This file provides functions to read in, trim and print 
 *     compressed or decompressed images.
 *
 ******************************************************************************/
#include "read_print.h"

struct convertedData {
        A2Methods_UArray2 converted;
        A2Methods_T methods;
};

static void apply_trim_ppm(int col, int row, 
                        A2Methods_UArray2 image, void *elem, void *cl);

static void apply_read_compressed(int col, int row, 
                        A2Methods_UArray2 image, void *elem, void *cl);

static void apply_print_compressed(int col, int row, 
                        A2Methods_UArray2 image, void *elem, void *cl);

/********** read_uncompressed ********
 *
 * Takes in an uncompresed file, trims it using a mapping function and 
 * returns the trimmed image as a 2D array.
 *
 * Parameters:
 *      FILE *fp: Takes in a file pointer to an uncompressed file
 *      A2Methods_mapfun *map: Includes pointer to mapping 
 *      functions to map through the file.
 *      A2Methods_T methods: Includes methods to build and use 2D arrays
 *      
 * Return: A trimmed PPM image.
 *
 * Expects
 *      Closure to not be null
 * Notes:
 *      TBD
 ************************/
Pnm_ppm read_uncompressed(FILE *fp, A2Methods_mapfun *map,
                        A2Methods_T methods)
{
        Pnm_ppm image = Pnm_ppmread(fp, methods);

        bool trim_width = (image->width % 2 != 0);
        bool trim_height = (image->height % 2 != 0);

        if (!trim_width && !trim_height) {
                return image;
        }

        if (trim_width) {
                image->width--;
        }

        if (trim_height) {
                image->height--;
        }

        struct convertedData *cl = malloc(sizeof(*cl));
        assert(cl != NULL);
        
        cl->converted = image->pixels;
        cl->methods = methods;

        A2Methods_UArray2 trimmed_pixels = methods->new(image->width, 
                image->height, sizeof(struct Pnm_rgb));

        map(trimmed_pixels, apply_trim_ppm, cl);

        methods->free(&(image->pixels));
        image->pixels = trimmed_pixels;

        free(cl);
        return image;
}

/********** apply_trim_ppm ********
 *
 * Takes in an compresed file index and copies it into a new 2D array with 
 * trimmed width and height.
 *
 * Parameters:
 *      
 *      int col: The column of the index
 *      int row: The row of the index
 *      A2Methods_UArray2 image: The 2D array of the ppm
 *      void *elem: The value at the index 
 *      void *cl: Passes in the methods and converted 2D array that will be 
 *      updated as a trimmed 2D array with an even width and height.
 *      
 * Return: A index copied into a converted trimmed ppm 2D array.
 *
 * Expects
 *      N/A
 * Notes:
 *      TBD
 ************************/
static void apply_trim_ppm(int col, int row, 
                        A2Methods_UArray2 image, void *elem, void *cl)
{
        (void) elem;
        struct convertedData *closure = (struct convertedData *)cl;

        A2Methods_T methods = closure->methods;
        A2Methods_UArray2 converted = closure->converted;

        Pnm_rgb new_pixel = methods->at(image, col, row);
        Pnm_rgb old_pixel = methods->at(converted, col, row);

        *new_pixel = *old_pixel;
}

/********** read_compressed ********
 *
 * Takes in an compresed file, and puts the words into a 2D array
 *
 * Parameters:
 *      FILE *fp: Takes in a file pointer to an uncompressed file
 *      A2Methods_mapfun *map: Includes pointer to mapping 
 *      functions to map through the file.
 *      A2Methods_T methods: Includes methods to build and use 2D arrays
 *      
 * Return: A trimmed PPM image.
 *
 * Expects
 *      Closure to not be null
 * Notes:
 *      TBD
 ************************/
A2Methods_UArray2 read_compressed(FILE *fp, A2Methods_mapfun *map,
                        A2Methods_T methods)
{
        unsigned height, width;
        int read = fscanf(fp, "COMP40 Compressed image format 2\n%u %u",
                                                        &width, &height);
        assert(read == 2);
        int c = getc(fp);
        assert(c == '\n');

        A2Methods_UArray2 compressed = methods->new(width / 2, height / 2,
                                                        sizeof(uint64_t));

        map(compressed, apply_read_compressed, fp);

        return compressed;
}

/********** apply_read_compressed ********
 *
 * Takes in an compresed file index and copies each 64 bitword into a 2D array.
 *
 * Parameters:
 *      
 *      int col: The column of the index
 *      int row: The row of the index
 *      A2Methods_UArray2 image: The 2D array of the ppm
 *      void *elem: The value at the index 
 *      void *cl: Passes in the methods and converted 2D array that will be 
 *      updated as a trimmed 2D array with an even width and height.
 *      
 * Return: A index copied into a converted 2D array of bitwords.
 *
 * Expects
 *      N/A
 * Notes:
 *      TBD
 ************************/
static void apply_read_compressed(int col, int row, 
                        A2Methods_UArray2 image, void *elem, void *cl)
{
        (void) image;
        (void) col;
        (void) row;

        FILE *fp = (FILE *)cl;
        assert(fp);

        uint64_t *word = (uint64_t *)elem;
        *word = 0;

        for (int shift = 24; shift >= 0; shift -= 8) {
                unsigned char byte = getc(fp);
                *word |= (uint64_t)byte << shift;
        }
}

/********** print_compressed ********
 *
 * Takes in a compressed 2D array and maps it to print each index out as a ppm
 *
 * Parameters:
 *      
 *      A2Methods_UArray2 image: Takes in a 2D array of a compressed image
 *      A2Methods_mapfun *map: Includes pointer to mapping 
 *      functions to map through the file.
 *      A2Methods_T methods: Includes methods to build and use 2D arrays
 *      
 * Return: N/A void function
 *
 * Expects
 *      Closure is not null
 * Notes:
 *      TBD
 ************************/
void print_compressed(A2Methods_UArray2 image, A2Methods_mapfun *map,
                        A2Methods_T methods)
{
        int width  = (methods->width(image)) * 2;
        int height = (methods->height(image)) * 2;

        printf("COMP40 Compressed image format 2\n%u %u", width, height);
        printf("\n");

        struct convertedData *cl = malloc(sizeof(*cl));
        assert(cl != NULL);

        cl->converted = image;
        cl->methods = methods;

        map(image, apply_print_compressed, cl);
        free(cl);
}

/********** apply_print_compressed ********
 *
 * Maps each index of a compressed 2D array and prints out each index
 *
 * Parameters:
 *      
 *      int col: The column of the index
 *      int row: The row of the index
 *      A2Methods_UArray2 image: The compressed 2D array of bits
 *      void *elem: The value at the index 
 *      void *cl: Passes in the methods and converted 2D array
 *      
 * Return: N/A void function
 *
 * Expects
 *      N/A
 * Notes:
 *      TBD
 ************************/
static void apply_print_compressed(int col, int row, 
                        A2Methods_UArray2 image, void *elem, void *cl)
{
        (void) image;
        (void) elem;

        struct convertedData *closure = (struct convertedData *)cl;

        A2Methods_T methods = closure->methods;
        A2Methods_UArray2 converted = closure->converted;

        uint64_t word = *(uint64_t *)methods->at(converted, col, row);
        for (int shift = 24; shift >= 0; shift -= 8) {
                unsigned char byte = (word >> shift) & 0xFF;
                putchar(byte);
        }
}