
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "rgb_cv_conversion.h"
#include "pack_unpack.h"
#include "a2plain.h"
#include "a2blocked.h"

struct convertedData {
        A2Methods_UArray2 converted;
        A2Methods_T methods;
        unsigned denominator;
};

A2Methods_UArray2 choose_size(Pnm_ppm image, A2Methods_mapfun *map, 
                        A2Methods_T methods);

                        static void apply_choose_size(int col, int row, A2Methods_UArray2 image, 
                                        void *elem, void *cl);

int main(int argc, char *argv[]) {

        FILE *fp;
        fp = fopen(argv[1], "r");
        
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods);
        
        /* default to best map */
        fprintf(stderr, "error 1\n");
        A2Methods_mapfun *map = methods->map_default; 
        fprintf(stderr, "error 2\n");
        Pnm_ppm image = Pnm_ppmread(fp, methods);
        fprintf(stderr, "error 3\n");

        A2Methods_UArray2 converted = choose_size(image, map, methods);

        Pnm_ppmwrite(stdout, converted);
        //methods->free(&words);
        //methods->free(&converted);
        //methods->free(&componentVid);
        //Pnm_ppmfree(&image);
        //Pnm_ppmfree(&decompressed);
        fclose(fp);
        (void) converted;
        (void) argc;
        return 0;
}


A2Methods_UArray2 choose_size(Pnm_ppm image, A2Methods_mapfun *map, 
                        A2Methods_T methods)
{
        A2Methods_UArray2 converted = methods->new(2, 2,
                                                        sizeof(struct Pnm_cv));

        struct convertedData *cl = malloc(sizeof(*cl));
        cl->converted = converted;
        cl->methods = methods;
        cl->denominator = image->denominator;

        map(image->pixels, apply_choose_size, cl);
        free(cl);

        return converted;
}

static void apply_choose_size(int col, int row, A2Methods_UArray2 image, 
                                        void *elem, void *cl)
{
        (void) image;
        struct convertedData *closure = (struct convertedData *)cl;

        A2Methods_T methods = closure->methods;
        unsigned denominator = closure->denominator;
        A2Methods_UArray2 converted = closure->converted;

        Pnm_rgb pixel = elem;

        Pnm_cv converted_pixel = malloc(sizeof(*converted_pixel));
        assert(converted_pixel != NULL);

        float red, green, blue;
        red   = pixel->red;
        green = pixel->green;
        blue  = pixel->blue;

        converted->red   = red;
        converted_pixel->green = green;
        converted_pixel->blue  = blue;

        *((Pnm_cv)(methods->at(converted, col, row))) = *converted_pixel;
        free(converted_pixel);


}

















int main(int argc, char *argv[]) {

        FILE *fp;
        fp = fopen(argv[1], "r");
        
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods);
        
        /* default to best map */
        fprintf(stderr, "error 1\n");
        A2Methods_mapfun *map = methods->map_default; 
        fprintf(stderr, "error 2\n");
        Pnm_ppm image = Pnm_ppmread(fp, methods);
        fprintf(stderr, "error 3\n");

        A2Methods_UArray2 converted = rgb_to_cv(image, map, methods);
        Pnm_ppm decompressed = cv_to_rgb(converted, map, methods);
        fprintf(stderr, "error 4\n");
        Pnm_ppmwrite(stdout, decompressed);
        // A2Methods_UArray2 words = cv_to_word(converted, map, methods);
        // fprintf(stderr, "error 5\n");
        // A2Methods_UArray2 bitpacked = word_to_bitpack(words, map, methods);
        // A2Methods_UArray2 unpacked = bitpack_to_word(bitpacked, map, methods);
        // fprintf(stderr, "error 7\n");
        // A2Methods_UArray2 componentVid = word_to_cv(unpacked, map, methods);



        // Pnm_ppm decompressed = cv_to_rgb(componentVid, map, methods);
        //(void) decompressed;
        
        //Pnm_ppmwrite(stdout, decompressed);
        //methods->free(&words);
        //methods->free(&converted);
        //methods->free(&componentVid);
        //Pnm_ppmfree(&image);
        //Pnm_ppmfree(&decompressed);
        fclose(fp);
        (void) converted;
        (void) argc;
        return 0;
}