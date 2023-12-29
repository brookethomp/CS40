#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "math.h"

unsigned int int_to_int(unsigned int k);
void get_sums(int col, int row, A2Methods_UArray2 image, void *elem, void *cl);

struct imageCurrSum {
        A2Methods_UArray2 imageCompare;
        A2Methods_T methods;
        double *TotalSum;
};

int main(int argc, char *argv[]) {


        FILE *fp1 = fopen(argv[1], "r");
        if (fp1 == NULL) {
                fprintf(stderr,
                        "Could not open file");
                exit(EXIT_FAILURE);
        }

        FILE *fp2 = fopen(argv[2], "r");
        if (fp2 == NULL) {
                fprintf(stderr,
                        "Could not open file");
                exit(EXIT_FAILURE);
        }

        A2Methods_T methods = uarray2_methods_plain;
        assert(methods);


        Pnm_ppm image1 = Pnm_ppmread(fp1, methods);
        Pnm_ppm image2 = Pnm_ppmread(fp2, methods);

        A2Methods_UArray2 image1_pixels = image1->pixels;
        A2Methods_UArray2 image2_pixels = image2->pixels;

        struct imageCurrSum *cl = malloc(sizeof(*cl));
        
        double TotalSum = 0;
        cl->methods = methods;
        cl->TotalSum = &TotalSum;
        cl->imageCompare = image2_pixels;

        // if (fp1 != NULL) {
        //     fp1 
        // }

        struct Pnm_rgb *image1pixel = (struct Pnm_rgb *)(methods->at(image1->pixels, 0, 0));
        struct Pnm_rgb *image2pixel = (struct Pnm_rgb *)(methods->at(image2->pixels, 0, 0));

        double red1scaled = image1pixel->red;
        red1scaled = red1scaled/255;

        double red2scaled = image2pixel->red/255;
        red2scaled = red2scaled/255;
        
        double redDiff = red1scaled - red1scaled;
        
        printf("Red 2 diff: %f\n", redDiff);
        double sumRed = pow((red1scaled - red2scaled), 2);

        printf("RedSum: %f\n", sumRed);
        printf("Image1: %d, %d, %d\n", image1pixel->red, image1pixel->blue, image1pixel->green);
        printf("Image2: %d, %d, %d\n", image2pixel->red, image2pixel->blue, image2pixel->green);

        methods->map_row_major(image1->pixels, (A2Methods_applyfun *) get_sums, cl);
        //methods->map_row_major(image2->pixels, (A2Methods_applyfun *) get_sums, cl);
        
        (void) image1_pixels;
        (void) image2_pixels;
        (void) argc;
        
        printf("toatl sum before dicide: %f\n", TotalSum);

        double denominator = 3 * image1->width * image1->height;
        double FinalE = sqrt(TotalSum/denominator);

        printf("final number: %f\n", FinalE);
        // (void *)(TotalSum);
        // printf("Total sum is: %p \n", cl->TotalSum);
        // //closure->TotalSum

        // int height1 = methods->height(image1);
        // int height2 = methods->height(image2);
        // if ((abs(height1 - height2) >= 0) && (abs(height1 - height2)<1) == false) {
        //         printf("height is not in range");
        //         exit(EXIT_FAILURE);
        // }

        // int width1 = methods->width(image1);
        // int width2 = methods->width(image2);
        // if ((abs(width1 - width2) >= 0) && (abs(width1 - width2)<1) == false) {
        //         printf("width is not in range");
        //         exit(EXIT_FAILURE);
        // }

}

void get_sums(int col, int row, A2Methods_UArray2 image, void *elem, void *cl) {

        (void) elem;
        struct imageCurrSum *closure = (struct imageCurrSum *)cl;
        A2Methods_T methods = closure->methods;
        A2Methods_UArray2 image2 = closure->imageCompare;

        double *TotalSum = closure->TotalSum;
        struct Pnm_rgb *image1pixel = (struct Pnm_rgb *)(methods->at(image, col, row));
        struct Pnm_rgb *image2pixel = (struct Pnm_rgb *)(methods->at(image2, col, row));

        double red1 = image1pixel->red;
        red1 = red1/255;

        double red2 = image2pixel->red;
        red2 = red2/255;
        
        double blue1 = image1pixel->blue;
        blue1 = blue1/255;

        double blue2 = image2pixel->blue;
        blue2 = blue2/255;

        double green1 = image1pixel->green;
        green1 = green1/255;
        
        double green2 = image2pixel->green;
        green2 = green2/255;

        double red = pow((red1) - (red2), 2);
        double blue = pow((blue1) - (blue2), 2);
        double green = pow((green1) - (green2), 2);

        double currSum = red + blue + green;

        *TotalSum = *TotalSum + currSum;
        printf("totalsum: %f\n", *TotalSum);
        //printf("col: %d, row: %d\n", col, row);
}