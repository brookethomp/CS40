/*******************************************************************************
 *
 *                     pack_unpack.c
 *
 *     Assignment: arith
 *     Authors: Brooke Thompson (bthomp05) and Richard Geoghegan (rgeogh01)
 *     Date:     10/20/23
 *
 *     This file provides the functions for packing and unpacking bits into 
 *      words and vice versa. 
 *
 ******************************************************************************/
#include "pack_unpack.h"

typedef struct constants {
        int width_a;
        int width_bcd;
        int width_pbpr;
        float max_bcd;
        int a_quant;
        int bcd_quant;   
} constants;

const constants GC = {9, 5, 4, 0.3, 511, 50};

typedef struct word {
        uint64_t a;
        int64_t b;
        int64_t c;
        int64_t d;
        uint64_t pb_bar;
        uint64_t pr_bar;    
} *word;

struct convertedData {
        A2Methods_UArray2 converted;
        A2Methods_T methods;
};

static void apply_cv_to_word(int col, int row, 
                        A2Methods_UArray2 image, void *elem, void *cl);

static void apply_word_to_cv(int col, int row, 
                        A2Methods_UArray2 image, void *elem, void *cl);

static void apply_word_to_bitpack(int col, int row, A2Methods_UArray2 image, 
                        void *elem, void *cl);

static void apply_bitpack_to_word(int col, int row, A2Methods_UArray2 image, 
                        void *elem, void *cl);

/********** quantize_a ********
 *
 * Quantizes 'a' by multiplying 'a' by 511 and then rounding to get 
 * 'a' in 9 unsigned bits.
 *
 * Parameters:
 *      float a: Takes float that is between 0-1
 *
 * Return: float 'a' as 9 unsigned bits
 *
 * Expects
 *      TBD
 * Notes:
 *      TBD
 ************************/
static uint64_t quantize_a(float a)
{
        a *= GC.a_quant;
        return (uint64_t)round(a);
}

/********** quantize_bcd ********
 *
 * Quantizes b, c, or d by round -0.5-0.5 to be in the range {-0.3-0.3} then 
 * multiplies by BCD_QUANT which is 50 to get a range of {-15-15} and 
 * turns b, c, or d into 5 unsigned bits each.
 *
 * Parameters:
 *      float bcd: Takes in the flaot of b c or d.
 *
 * Return: A quantized b, c or d value in 5 bits.
 *
 * Expects
 *      TBD
 * Notes:
 *      TBD
 ************************/
static int64_t quantize_bcd(float bcd)
{
        if (bcd > GC.max_bcd) {
                bcd = GC.max_bcd;
        } else if (bcd < -GC.max_bcd) {
                bcd = -GC.max_bcd;
        }
        return (int64_t)round(bcd * GC.bcd_quant);
}

/********** unquantize_a ********
 *
 * Takes a as 9 unisgned bits and unquantizes it into a number between 0 and 1 
 * to get the average brightness of the image.
 *
 * Parameters:
 *      uint64_t a: Takes in an 9 unsigned bits
 *
 * Return: returns a number between 0 and 1 to get the average brightness 
 * of the image.
 *
 * Expects
 *      TBD
 * Notes:
 *      TBD
 ************************/
static float unquantize_a(uint64_t a)
{
        return (float)a / (float)GC.a_quant;
}

/********** unquantize_bcd ********
 *
 * Takes in 5 unisgned bits and unquantizes by dividing it by 50 to get a 
 * number between {-0.3-0.3}
 *
 * Parameters:
 *      uint64_t a: Takes in 5 unsigned bits
 *
 * Return: returns a number between -0.3 and 0.3 to get the average brightness 
 * of the image.
 *
 * Expects
 *      TBD
 * Notes:
 *      TBD
 ************************/
static float unquantize_bcd(int64_t bcd) 
{
    return (float)bcd / (float)GC.bcd_quant;
}

/********** bound_y ********
 *
 * Takes in Yi and converts the Yi into a number between 0 and 1. Rounds up 
 * if Yi is less than zero, rounds down to 1 if Yi is greater than 0.
 *
 * Parameters:
 *      float y: Takes in float
 *
 * Return: returns a number between 0-1 as the new Y value.
 *
 * Expects
 *      TBD
 * Notes:
 *      TBD
 ************************/
static float bound_y(float y)
{
        if (y > 1.0f) {
                y = 1.0f;
        } else if (y < 0.0f) {
                y = 0.0f;
        }
        return y;
}

/********** cv_to_word ********
 *
 * Takes in a 2D array of structs that hold compoenet-video values then maps 
 * and runs an apply function to each index. Returns
 * a 2D array of structs that have the quantized values and represent 
 * each value as a segment of bits.
 *
 * Parameters:
 *      A2Methods_UArray2 image: 2D array of structs of component-video values
 *      A2Methods_mapfun *map: Map function to traverse through
 *      each index and run an apply function
 *      A2Methods_T methods: Takes in the methods for 2D arrays 
 *
 * Return: returns a 2D array of structs of segmets of bits.
 *
 * Expects
 *      Closure to not be null
 * Notes:
 *      TBD
 ************************/
A2Methods_UArray2 cv_to_word(A2Methods_UArray2 image, 
                        A2Methods_mapfun *map, A2Methods_T methods)
{ 
        int width  = (methods->width(image));
        int height = (methods->height(image));

        A2Methods_UArray2 converted = methods->new(width / 2, height / 2,
                                                        sizeof(struct word));

        struct convertedData *cl = malloc(sizeof(*cl));
        assert(cl != NULL);

        cl->converted = image;
        cl->methods = methods;

        map(converted, apply_cv_to_word, cl);
        free(cl);

        return converted;
}

/********** apply_cv_to_word ********
 *
 * Takes in an index of a 2D array that has a struct of component-video values 
 * and runs calculations on each value in the 
 * struct of the indedx to turn Y values into quantized a, b ,c 
 * and d values and quantizes Pb and Pr so that they are represented by bits.
 *
 * Parameters:
 *      int col: The column of the index
 *      int row: The row of the index
 *      A2Methods_UArray2 image: The 2D array of 
 *      structs of component-video values
 *      void *elem: The value at the index that holds a struct of 
 *      component-video values
 *      void *cl: Passes in the methods and converted 2D array that will be 
 *      updated with a struct of segmets of bits.
 *
 * Return: N/A
 *
 * Expects
 *      TBD
 * Notes:
 *      TBD
 ************************/
static void apply_cv_to_word(int col, int row, 
                        A2Methods_UArray2 image, void *elem, void *cl)
{
        (void) image;
        struct convertedData *closure = (struct convertedData *)cl;

        A2Methods_T methods = closure->methods;
        A2Methods_UArray2 converted = closure->converted;
        (void) converted;

        col *= 2;
        row *= 2;

        Pnm_cv cv1, cv2, cv3, cv4;
        cv1 = methods->at(converted, col, row);
        cv2 = methods->at(converted, col + 1, row);
        cv3 = methods->at(converted, col, row + 1);
        cv4 = methods->at(converted, col + 1, row + 1);
        
        word curr_word = elem;

        float a, b, c, d, pb_bar, pr_bar;
        a = (cv4->y + cv3->y + cv2->y + cv1->y) / 4.0f;
        b = (cv4->y + cv3->y - cv2->y - cv1->y) / 4.0f;
        c = (cv4->y - cv3->y + cv2->y - cv1->y) / 4.0f;
        d = (cv4->y - cv3->y - cv2->y + cv1->y) / 4.0f;
        pb_bar = (cv1->pb + cv2->pb + cv3->pb + cv4->pb) / 4.0f;
        pr_bar = (cv1->pr + cv2->pr + cv3->pr + cv4->pr) / 4.0f;

        curr_word->a = quantize_a(a);
        curr_word->b = quantize_bcd(b);
        curr_word->c = quantize_bcd(c);
        curr_word->d = quantize_bcd(d);
        curr_word->pb_bar = (uint64_t)(Arith40_index_of_chroma(pb_bar));
        curr_word->pr_bar = (uint64_t)(Arith40_index_of_chroma(pr_bar));
}

/********** word_to_cv ********
 *
 * Takes in a 2D array of structs that holds segments of bits then maps 
 * and runs an apply function to each index. Returns
 * a 2D array of structs that have the component-video 
 * values of Y1, Y2, Y3, Y4, Pb and Pr.
 *
 * Parameters:
 *      A2Methods_UArray2 image: 2D array of structs of component-video values
 *      A2Methods_mapfun *map: Map function to traverse through
 *      each index and run an apply function
 *      A2Methods_T methods: Takes in the methods for 2D arrays 
 *
 * Return: returns a 2D array of structs of component-video values.
 *
 * Expects
 *      Closure to not be null
 * Notes:
 *      TBD
 ************************/
A2Methods_UArray2 word_to_cv(A2Methods_UArray2 image, 
                        A2Methods_mapfun *map, A2Methods_T methods)
{
        int width  = (methods->width(image));
        int height = (methods->height(image));

        A2Methods_UArray2 converted = methods->new(width * 2, height * 2,
                                                        sizeof(struct Pnm_cv));

        struct convertedData *cl = malloc(sizeof(*cl));
        assert(cl != NULL);

        cl->converted = converted;
        cl->methods = methods;

        map(image, apply_word_to_cv, cl);
        free(cl);

        return converted;
}

/********** apply_word_to_cv ********
 *
 * Takes in an index of a 2D array that has a struct of segments of bits
 * and runs calculations on each value in the 
 * struct of the indedx to turn a, b ,c 
 * and d values and quantized Pb and Pr into Y, Pb and Pr values 
 * as component-video values.
 *
 * Parameters:
 *      int col: The column of the index
 *      int row: The row of the index
 *      A2Methods_UArray2 image: The 2D array of structs of segments of bits
 *      void *elem: The value at the index that holds a struct of 
 *      segments of bits
 *      void *cl: Passes in the methods and converted 2D array that will be 
 *      updated with a struct of component video values.
 *
 * Return: N/A
 *
 * Expects
 *      TBD
 * Notes:
 *      TBD
 ************************/
static void apply_word_to_cv(int col, int row, 
                        A2Methods_UArray2 image, void *elem, void *cl)
{
        (void) image;
        struct convertedData *closure = (struct convertedData *)cl;

        A2Methods_T methods = closure->methods;
        A2Methods_UArray2 converted = closure->converted;

        Pnm_cv cvs[4];
        cvs[0] = methods->at(converted, 2 * col, 2 * row);
        cvs[1] = methods->at(converted, 2 * col + 1, 2 * row);
        cvs[2] = methods->at(converted, 2 * col, 2 * row + 1);
        cvs[3] = methods->at(converted, 2 * col + 1, 2 * row + 1);
    
        word curr_word = elem;

        float a = unquantize_a(curr_word->a);
        float b = unquantize_bcd(curr_word->b);
        float c = unquantize_bcd(curr_word->c);
        float d = unquantize_bcd(curr_word->d);
        float pb_bar = (float)Arith40_chroma_of_index(curr_word->pb_bar);
        float pr_bar = (float)Arith40_chroma_of_index(curr_word->pr_bar);

        float ys[4] = {
                bound_y(a - b - c + d),
                bound_y(a - b + c - d),
                bound_y(a + b - c - d),
                bound_y(a + b + c + d)
        };

        for (int i = 0; i < 4; i++) {
                cvs[i]->pb = pb_bar;
                cvs[i]->pr = pr_bar;
                cvs[i]->y = ys[i];
        }
}

/********** word_to_bitpack ********
 *
 * Takes in a 2D array of structs that hold unsigned bits then maps 
 * and runs an apply function to each index. Returns
 * a 2D array of words at each index. The words of comprised of the 
 * bits packed into a 64 unsigned bitpack.
 *
 * Parameters:
 *      A2Methods_UArray2 image: 2D array of structs of bits
 *      A2Methods_mapfun *map: Map function to traverse through each index and run an apply function
 *      A2Methods_T methods: Takes in the methods for 2D arrays 
 *
 * Return: returns a 2D array of 64 bit words at each index.
 *
 * Expects
 *      Closure to not be null
 * Notes:
 *      TBD
 ************************/
A2Methods_UArray2 word_to_bitpack(A2Methods_UArray2 image, 
                        A2Methods_mapfun *map, A2Methods_T methods)
{
        int width  = (methods->width(image));
        int height = (methods->height(image));

        A2Methods_UArray2 converted = methods->new(width, height,
                                                        sizeof(uint64_t));

        struct convertedData *cl = malloc(sizeof(*cl));
        assert(cl != NULL);

        cl->converted = converted;
        cl->methods = methods;

        map(image, apply_word_to_bitpack, cl);
        free(cl);

        return converted;
}

/********** apply_word_to_bitpack ********
 *
 * Takes in an index of a 2D array that has a struct of bits representations 
 * and packs each group of bits into a 64 bit word.
 *
 * Parameters:
 *      int col: The column of the index
 *      int row: The row of the index
 *      A2Methods_UArray2 image: The 2D array of structs of each part of 
 *      the word as bits
 *      void *elem: The value at the index that holds a structs of each part of 
 *      the word as bits
 *      void *cl: Passes in the methods and converted 2D array that will be 
 *      updated with a words of thats bitpacked into 64 bits.
 *
 * Return: N/A
 *
 * Expects
 *      TBD
 * Notes:
 *      TBD
 ************************/
static void apply_word_to_bitpack(int col, int row, A2Methods_UArray2 image, 
                        void *elem, void *cl)
{
        (void) image;
        struct convertedData *closure = (struct convertedData *)cl;

        A2Methods_T methods = closure->methods;
        A2Methods_UArray2 converted = closure->converted;

        word curr_word = elem;

        uint64_t bitpacked_word = 0;

        bitpacked_word = Bitpack_newu(bitpacked_word, GC.width_pbpr,
                0, curr_word->pr_bar);

        bitpacked_word = Bitpack_newu(bitpacked_word, GC.width_pbpr,
                GC.width_pbpr, curr_word->pb_bar);

        bitpacked_word = Bitpack_news(bitpacked_word, GC.width_bcd,
                2 * GC.width_pbpr, curr_word->d);

        bitpacked_word = Bitpack_news(bitpacked_word, GC.width_bcd,
                (2 * GC.width_pbpr) + GC.width_bcd, curr_word->c);

        bitpacked_word = Bitpack_news(bitpacked_word, GC.width_bcd,
                (2 * GC.width_pbpr) + (2 * GC.width_bcd), curr_word->b);

        bitpacked_word = Bitpack_newu(bitpacked_word, GC.width_a, 
                (2 * GC.width_pbpr) + (3 * GC.width_bcd), curr_word->a);

        *((uint64_t*)(methods->at(converted, col, row))) = bitpacked_word;
}

/********** bitpack_to_word ********
 *
 * Takes in a 2D array of a 64 bitpacked word then maps 
 * and runs an apply function to each index. Returns
 * a 2D array of structs that hold the upacked unsigned bits 
 * that made up the word.
 *
 * Parameters:
 *      A2Methods_UArray2 image: 2D array of 64 bits
 *      A2Methods_mapfun *map: Map function to traverse through each index and run an apply function
 *      A2Methods_T methods: Takes in the methods for 2D arrays 
 *
 * Return: returns a 2D array of structs that hold bits.
 *
 * Expects
 *      Closure to not be null
 * Notes:
 *      TBD
 ************************/
A2Methods_UArray2 bitpack_to_word(A2Methods_UArray2 image,
                        A2Methods_mapfun *map, A2Methods_T methods)
{
        int width  = (methods->width(image));
        int height = (methods->height(image));

        A2Methods_UArray2 converted = methods->new(width, height,
                                                        sizeof(struct word));

        struct convertedData *cl = malloc(sizeof(*cl));
        assert(cl != NULL);

        cl->converted = converted;
        cl->methods = methods;

        map(image, apply_bitpack_to_word, cl);
        free(cl);

        return converted;
}

/********** apply_bitpack_to_word ********
 *
 * Takes in an index of a 2D array with 64 bit words and unbitpacks each word 
 * into a struct that holds bits.
 *
 * Parameters:
 *      int col: The column of the index
 *      int row: The row of the index
 *      A2Methods_UArray2 image: The 2D array of 64 bit words
 *      void *elem: The value at the index that holds a 64 bit word
 *      void *cl: Passes in the methods and converted 2D array that will be 
 *      updated with upacked bits.
 *
 * Return: N/A
 *
 * Expects
 *      TBD
 * Notes:
 *      TBD
 ************************/
static void apply_bitpack_to_word(int col, int row, A2Methods_UArray2 image, 
                        void *elem, void *cl)
{
        (void) image;
        struct convertedData *closure = (struct convertedData *)cl;

        A2Methods_T methods = closure->methods;
        A2Methods_UArray2 converted = closure->converted;

        uint64_t bitpacked_word = *(uint64_t *)elem;
        word curr_word = methods->at(converted, col, row);

        curr_word->a  = Bitpack_getu(bitpacked_word, GC.width_a,
                (2 * GC.width_pbpr) + (3 * GC.width_bcd));
        curr_word->b  = Bitpack_gets(bitpacked_word, GC.width_bcd,
                (2 * GC.width_pbpr) + (2 * GC.width_bcd));
        curr_word->c  = Bitpack_gets(bitpacked_word, GC.width_bcd,
                (2 * GC.width_pbpr)+ GC.width_bcd);
        curr_word->d  = Bitpack_gets(bitpacked_word, GC.width_bcd,
                2 * GC.width_pbpr);
        curr_word->pb_bar = Bitpack_getu(bitpacked_word, GC.width_pbpr,
                GC.width_pbpr);
        curr_word->pr_bar = Bitpack_getu(bitpacked_word, GC.width_pbpr, 0);
}