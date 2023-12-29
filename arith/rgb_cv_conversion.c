/*****************************************************************************
 *
 *                     rgb_cv_conversion.c
 *
 *     Assignment: arith
 *     Authors: Brooke Thompson (bthomp05) and Richard Geoghegan(rgeogh01)
 *     Date: 10/20/23
 *
 *     This file provides the implementation for rgb_cv_conversion. This
 *     will either take in a ppm image and convert it to a 2D array containing
 *     the component video representation at each RGB pixel or it will do
 *     the reverse.
 *
 ****************************************************************************/
#include "rgb_cv_conversion.h"

#define DENOMINATOR 255

struct convertedData {
        A2Methods_UArray2 converted;
        A2Methods_T methods;
        unsigned denominator;
};

static void apply_rgb_to_cv(int col, int row, A2Methods_UArray2 image, 
                                        void *elem, void *cl);

static void apply_cv_to_rgb(int col, int row, A2Methods_UArray2 image, 
                                        void *elem, void *cl);
/********** bound_rgb ********
 *
 * Given an RGB value, bound_rgb will set it to 0 if it's below 0 and
 * 255 if it's above the denominator.
 *
 * Parameters:
 *      float rgb: Takes in a float for the red, blue or green value of a 
 *      pixel.
 *
 * Return: A red, blue or green value thats within the bounds of 0-Denominator.
 *
 * Expects:
 *      None
 * Notes:
 *      None
 ************************/
static float bound_rgb(float rgb)
{
        if (rgb > DENOMINATOR) {
                rgb = DENOMINATOR;
        } else if (rgb < 0.0f) {
                rgb = 0.0f;
        }
      
        return rgb;
}

/********** rgb_to_cv ********
 *
 * Converts the provided ppm image to a 2D array containing the component
 * video representation of each pixel.
 *
 * Parameters:
 *      Pnm_ppm image: The image to be converted.
 *      A2Methods_mapfun *map: A pointer to the A2Methods mapping function.
 *      A2Methods_T methods: An A2Methods_T struct containing the required
 *                           methods.
 *
 * Return: A 2D array of component-video structs at each cell.
 *
 * Expects:
 *      Map to not be NULL
 * Notes:
 *      TBD
 ************************/                            
A2Methods_UArray2 rgb_to_cv(Pnm_ppm image, A2Methods_mapfun *map, 
                        A2Methods_T methods)
{
        A2Methods_UArray2 converted = methods->new(image->width, image->height,
                                                        sizeof(struct Pnm_cv));

        struct convertedData *cl = malloc(sizeof(*cl));
        assert(cl != NULL);

        cl->converted = converted;
        cl->methods = methods;
        cl->denominator = image->denominator;

        map(image->pixels, apply_rgb_to_cv, cl);
        free(cl);

        return converted;
}

/********** apply_rgb_to_cv ********
 *
 * Turns RGB values into component-video values at each index of a 2D 
 * array and stores them in a new 2D array.
 *
 * Parameters:
 *      int col: The column of the 2D array the function is mapped at
 *      int row: The row of the 2D array the function is mapped at
 *      A2Methods_UArray2 image: Takes in the 2D array of the image 
 *      void *elem: Current index
 *      void *cl: Struct containing a new A2Array and the A2Methods_T methods
 *                and the denominator.
 *
 * Return: Updates the values in the converted 2D array to hold the newly
 *         calculated cv values.
 *
 * Expects
 *      cl to not be NULL
 * Notes:
 *      TBD
 ************************/
static void apply_rgb_to_cv(int col, int row, A2Methods_UArray2 image, 
                                        void *elem, void *cl)
{
        (void) image;
        struct convertedData *closure = (struct convertedData *)cl;

        A2Methods_T methods = closure->methods;
        unsigned denominator = closure->denominator;
        A2Methods_UArray2 converted = closure->converted;

        Pnm_rgb pixel = elem;

        float red, green, blue;
        red   = pixel->red / (float) denominator;
        green = pixel->green / (float) denominator;
        blue  = pixel->blue / (float) denominator;

        Pnm_cv converted_pixel = malloc(sizeof(*converted_pixel));
        assert(converted_pixel != NULL);

        converted_pixel->y  = (0.299 * red) + (0.587 * green) + 
                                                (0.114 * blue);
        converted_pixel->pb = (-0.168736 * red) - (0.331264 * green) + 
                                                (0.5 * blue);
        converted_pixel->pr = (0.5 * red) - (0.418688 * green) - 
                                                (0.081312 * blue);

        *((Pnm_cv)(methods->at(converted, col, row))) = *converted_pixel;
        free(converted_pixel);
}

/********** cv_to_rgb ********
 *
 * Makes a new 2D array then maps the original compressed 2D array of 
 * Pnm_rgb structs with an apply function 
 * on each index. Returns the 2D array of Pnm_cv structs that 
 * were converted from rgb.
 *
 * Parameters:
 *      A2Methods_UArray2 image: 2D array containing Pnm_cv structs.
 *      A2Methods_mapfun *map: A pointer to the A2Methods mapping function.
 *      A2Methods_T methods: An A2Methods_T struct containing the required
 *                           methods.
 *
 * Return: A converted 2D array of RGB structs at each cell
 *
 * Expects:
 *      Map to not be NULL
 * Notes:
 *      TBD
 ************************/
Pnm_ppm cv_to_rgb(A2Methods_UArray2 image, A2Methods_mapfun *map, 
                        A2Methods_T methods)
{
        int width  = (methods->width(image));
        int height = (methods->height(image));
        A2Methods_UArray2 converted = methods->new(width, height,
                                        sizeof(struct Pnm_rgb));

        struct convertedData *cl = malloc(sizeof(*cl));
        assert(cl != NULL);

        cl->converted   = converted;
        cl->methods     = methods;
        cl->denominator = DENOMINATOR;

        Pnm_ppm image_rgb = malloc(sizeof(*image_rgb));
        assert(image_rgb != NULL);

        image_rgb->width       = width;
        image_rgb->height      = height;
        image_rgb->denominator = cl->denominator;
        image_rgb->methods     = methods;
        image_rgb->pixels      = converted;
        
        map(image, apply_cv_to_rgb, cl);
        free(cl);

        return image_rgb;
}

/********** apply_cv_to_rgb ********
 *
 * Turns component-video values values into RGB values at each index of a 2D 
 * array and stores them in a new 2D array.
 *
 * Parameters:
 *      int col: The column of the 2D array the function is mapped at
 *      int row: The row of the 2D array the function is mapped at
 *      A2Methods_UArray2 image: Takes in a 2D array of Pnm_rgb structs
 *      void *elem:       Takes in a pixel at each index
 *      void *cl:         Passes the converted array, methods of A2methods and 
 *                        the denominator of the image through a closure so 
 *                        that they do not reset every index.
 *
 * Return: A converted 2D array with an index changed to a RGB 
 * struct each time the apply function is applied.
 *
 * Expects
 *      cl to not be NULL
 * Notes:
 *      TBD
 ************************/
static void apply_cv_to_rgb(int col, int row, A2Methods_UArray2 image, 
                                        void *elem, void *cl)
{
        (void) image;
        struct convertedData *closure = (struct convertedData *)cl;

        A2Methods_T methods = closure->methods;
        unsigned denominator = closure->denominator;
        A2Methods_UArray2 converted = closure->converted;

        Pnm_cv curr_pixel = elem;
        float y  = curr_pixel->y;
        float pb = curr_pixel->pb;
        float pr = curr_pixel->pr;

        Pnm_rgb converted_pixel = malloc(sizeof(*converted_pixel));
        assert(converted_pixel != NULL);

        float red, green, blue;
        red   = denominator * ((1.0 * y) + (0.0 * pb) + (1.402 * pr));
        green = denominator * ((1.0 * y) - (0.344136 * pb) - (0.714136 * pr));
        blue  = denominator * ((1.0 * y) + (1.772 * pb) + (0.0 * pr));
        
        converted_pixel->red   = (unsigned)bound_rgb(red);
        converted_pixel->green = (unsigned)bound_rgb(green);
        converted_pixel->blue  = (unsigned)bound_rgb(blue);

        *((Pnm_rgb)(methods->at(converted, col, row))) = *converted_pixel;
        free(converted_pixel);
}