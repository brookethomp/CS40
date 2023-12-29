/*******************************************************************************
 *
 *                     ppmtrans.c
 *
 *     Assignment: locality
 *     Authors: Brooke Thompson (bthomp05) and Ian Ryan (iryan01)
 *     Date:     10/10/23
 *
 *     This file provides a program that...
 *
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "uarray2b.h"
#include "uarray2.h"
#include "pnm.h"
#include "cputiming.h"

#define SET_METHODS(METHODS, MAP, WHAT) do {                    \
        methods = (METHODS);                                    \
        assert(methods != NULL);                                \
        map = methods->MAP;                                     \
        if (map == NULL) {                                      \
                fprintf(stderr, "%s does not support "          \
                                WHAT "mapping\n",               \
                                argv[0]);                       \
                exit(1);                                        \
        }                                                       \
} while (false)

static void
usage(const char *progname)
{
        fprintf(stderr, "Usage: %s [-rotate <angle>] "
                        "[-{row,col,block}-major] [filename]\n",
                        progname);
        exit(1);
}

static FILE *open_or_abort(char *fname, char *mode);
void apply_blocked90(int applyCol, int applyRow, UArray2b_T a, void *elem, void *t);
void apply_blocked180(int applyCol, int applyRow, UArray2b_T a, void *elem, void *t);
void apply_2D90(int applyCol, int applyRow, UArray2_T a, void *elem, void *t);
void apply_2D180(int applyCol, int applyRow, UArray2_T a, void *elem, void *t);
void apply_row_270(int applyCol, int applyRow, UArray2_T a, void *elem, void *t);

struct A2methods_struct {
        A2Methods_UArray2 A2;
        A2Methods_T methods;
};

int main(int argc, char *argv[]) 
{
        char *time_file_name = NULL;
        int   rotation       = 0;
        int   i;

        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods);        

        /* default to best map */
        A2Methods_mapfun *map = methods->map_default; 
        assert(map);

        int majorCall;
        int rotationCall;

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-row-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_row_major, 
                                    "row-major");
                        majorCall = 0;
                } else if (strcmp(argv[i], "-col-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_col_major, 
                                    "column-major");
                        majorCall = 1;
                } else if (strcmp(argv[i], "-block-major") == 0) {
                        SET_METHODS(uarray2_methods_blocked, map_block_major,
                                    "block-major");
                        majorCall = 2;
                } else if (strcmp(argv[i], "-rotate") == 0) {
                        if (!(i + 1 < argc)) {
                                usage(argv[0]);
                                rotationCall = 0;
                        }
                        char *endptr;
                        rotation = strtol(argv[++i], &endptr, 10);
                        if (!(rotation == 0 || rotation == 90 ||
                            rotation == 180 || rotation == 270)) {
                                fprintf(stderr, 
                                        "Rotation must be 0, 90 180 or 270\n");
                                usage(argv[0]);
                        }
                        if (!(*endptr == '\0')) {
                                usage(argv[0]);
                        }
                } else if (strcmp(argv[i], "-time") == 0) {
                        time_file_name = argv[++i];
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                                argv[i]);
                        usage(argv[0]);
                } else if (argc - i > 1) {
                        fprintf(stderr, "Too many arguments\n");
                        usage(argv[0]);
                } else {
                        rotationCall = rotation;
                        break;
                }
        }

        // (void)time_file_name;
        FILE *timings_file;
        CPUTime_T timer;
        timer = CPUTime_New();
        double time_used;
        bool open = false; 
        
        if (time_file_name != NULL){
               timings_file = fopen(time_file_name, "a"); 
               open = true;
        }
        
        FILE *fp;
        if (open){
                if (argc == 6) {
                        fp = stdin;
                } else {
                        fp = open_or_abort(argv[6], "r");
                }   
        } else {
                if (argc == 4) {
                        fp = stdin;
                } else {
                        fp = open_or_abort(argv[4], "r");
                }
        }  

        Pnm_ppm reader = Pnm_ppmread(fp, methods);
        A2Methods_UArray2 A2;
        struct A2methods_struct *holder = malloc(sizeof (struct A2methods_struct));
        holder->methods = methods;
        
        if (majorCall == 0){
                if (rotationCall == 90) {
                        A2 = methods->new((int)reader->height, (int)reader->width, sizeof(struct Pnm_rgb));
                        holder->A2 = A2;
                        if (open){
                                CPUTime_Start(timer);
                        }
                        methods->map_row_major(reader->pixels, (A2Methods_applyfun *) apply_2D90, holder);
                        if (open){
                                time_used = CPUTime_Stop(timer);
                        }
                } else if (rotationCall == 180) {
                        A2 = methods->new((int)reader->width, (int)reader->height, sizeof(struct Pnm_rgb));
                        holder->A2 = A2;
                        if (open){
                                CPUTime_Start(timer);
                        }
                        methods->map_row_major(reader->pixels, (A2Methods_applyfun *) apply_2D180, holder);
                        if (open){
                                time_used = CPUTime_Stop(timer);
                        }
                } else if (rotationCall == 270) {
                        A2 = methods->new((int)reader->height, (int)reader->width, sizeof(struct Pnm_rgb));
                        holder->A2 = A2;
                        if (open){
                                CPUTime_Start(timer);
                        }
                        methods->map_row_major(reader->pixels, (A2Methods_applyfun *) apply_row_270, holder);
                        if (open){
                                time_used = CPUTime_Stop(timer);
                        }
                }
        } else if (majorCall == 1) {
                if (rotationCall == 90) {
                        A2 = methods->new((int)reader->height, (int)reader->width, sizeof(struct Pnm_rgb));
                        holder->A2 = A2;
                        if (open){
                                CPUTime_Start(timer);
                        }
                        methods->map_col_major(reader->pixels, (A2Methods_applyfun *) apply_2D90, holder);
                        if (open){
                                time_used = CPUTime_Stop(timer);
                        }
                } else if (rotationCall == 180) {
                        A2 = methods->new((int)reader->width, (int)reader->height, sizeof(struct Pnm_rgb));
                        holder->A2 = A2;
                        if (open){
                                CPUTime_Start(timer);
                        }
                        methods->map_col_major(reader->pixels, (A2Methods_applyfun *) apply_2D180, holder);
                        if (open){
                                time_used = CPUTime_Stop(timer);
                        }
                }       
        } else if (majorCall == 2) {
                if (rotationCall == 90) {
                        A2 = methods->new((int)reader->height, (int)reader->width, sizeof(struct Pnm_rgb));
                        holder->A2 = A2;
                        if (open){
                                CPUTime_Start(timer);
                        }
                        methods->map_block_major(reader->pixels, (A2Methods_applyfun *) apply_blocked90, holder);
                        if (open){
                                time_used = CPUTime_Stop(timer);
                        }
                } else if (rotationCall == 180) {
                        A2 = methods->new((int)reader->width, (int)reader->height, sizeof(struct Pnm_rgb));
                        holder->A2 = A2;
                        if (open){
                                CPUTime_Start(timer);
                        }
                        methods->map_block_major(reader->pixels, (A2Methods_applyfun *) apply_blocked180, holder);
                        if (open){
                                time_used = CPUTime_Stop(timer);
                        }
                }
        }
        
        // (void)time_used;
        
        if (rotationCall != 0){
                A2Methods_UArray2 pix = reader->pixels;
                methods->free(&pix);
                reader->pixels = A2;
        }
        if (rotationCall == 90) {
                reader->width = (unsigned)methods->width(A2);
                reader->height = (unsigned)methods->height(A2);
        }        
        if (open){
                int pixels = methods->height(A2) * methods->width(A2);
                fprintf(timings_file, "height: %d, width: %d, #pixels: %d\n", methods->height(A2), methods->width(A2), pixels);
                double nano = time_used/pixels;
                fprintf(timings_file, "rotation %d, mapping method %d, computed in %f nanoseconds\n", rotationCall, majorCall, time_used);
                fprintf(timings_file, "time per unit pixel: %f nanoseconds\n", nano);
                CPUTime_Free(&timer);
        }
        FILE *out_file = fopen("output.ppm", "w");
        Pnm_ppmwrite(out_file, reader);
                        
        //free all associated memory
        //NEED TO FREE SOMEThING TO DO WITH TIMING
        free(holder);
        fclose(fp);
        fclose(out_file);
        Pnm_ppmfree(&reader);
        
        return EXIT_SUCCESS;
}

/********** open_or_abort ********
 *
 * Returns an open file object or calls a checked runtime error if the file is
 * unable to be opened.
 *
 * Parameters:
 *      char *fname:            name of the file to be opened
 *      char *mode:             the mode for opening the file
 *
 * Return: an open file object if it is able to be opened
 *
 * Expects
 *      the file passed through fname is able to be opened.
 * Notes:
 *      Will CRE if the file is unable to be opened
 ************************/
static FILE *open_or_abort(char *fname, char *mode) 
{
        FILE *fp = fopen(fname, mode);
        if (fp == NULL) {
                fprintf(stderr,
                        "Could not open file %s with mode %s\n",
                        fname,
                        mode);
                exit(EXIT_FAILURE);
        }
        return fp;
}

/********** apply_blocked90 ********
 *
 * An apply function to be called by the block-major mapping function that
 * rotates the given UArray2 by 90 degrees clockwise.
 *
 * Parameters:
 *      int applyCol:            name of the file to be opened
 *      int applyCol:            the mode for opening the file
 *      UArray2b_T a:            a pointer to a UArray2b_T struct.
 *      void *elem:              a pointer to the current element in the
 *                               mapping function
 *      void *t:                 a void pointer, which in this case is expected
 *                               to be a pointer to a UArray2b_T struct
 *
 * Return: void.
 *
 * Expects
 *      TBD
 * Notes:
 *      TBD
 ************************/
void apply_blocked90(int applyCol, int applyRow, UArray2b_T a, void *elem, 
                     void *t) 
{
        struct A2methods_struct *holder = t;
        A2Methods_T methods = holder->methods;
        int height = methods->height(a);
        // int height = UArray2b_height(a);
        int new_col = (height - applyRow - 1);
        int new_row = applyCol;

        // Pnm_rgb new_location = methods->at(t, new_col, new_row);
        Pnm_rgb new_location = methods->at(holder->A2, new_col, new_row);
        Pnm_rgb x = elem;
        new_location->red = x->red;
        new_location->green = x->green;
        new_location->blue = x->blue;
}

/********** apply_blocked180 ********
 *
 * An apply function to be called by the block-major mapping function that
 * rotates the given UArray2 by 180 degrees clockwise.
 *
 * Parameters:
 *      int applyCol:            name of the file to be opened
 *      int applyCol:            the mode for opening the file
 *      UArray2b_T a:            a pointer to a UArray2b_T struct.
 *      void *elem:              a pointer to the current element in the
 *                               mapping function
 *      void *t:                 a void pointer, which in this case is expected
 *                               to be a pointer to a UArray2b_T struct
 *
 * Return: void.
 *
 * Expects
 *      TBD
 * Notes:
 *      TBD
 ************************/
void apply_blocked180(int applyCol, int applyRow, UArray2b_T a, void *elem,
                      void *t) 
{
        struct A2methods_struct *holder = t;
        A2Methods_T methods = holder->methods;
        int height = methods->height(a);
        int width = methods->width(a);
        int new_col = (width - applyCol - 1);
        int new_row = (height - applyRow - 1);

        // Pnm_rgb new_location = methods->at(t, new_col, new_row);
        Pnm_rgb new_location = methods->at(holder->A2, new_col, new_row);
        Pnm_rgb x = elem;
        new_location->red = x->red;
        new_location->green = x->green;
        new_location->blue = x->blue;

}

/********** apply_2D90 ********
 *
 * An apply function to be called by the row-major or col-major mapping
 * function that rotates the given UArray2 by 90 degrees clockwise.
 *
 * Parameters:
 *      int applyCol:            name of the file to be opened
 *      int applyCol:            the mode for opening the file
 *      UArray2_T a:             a pointer to a UArray2_T struct.
 *      void *elem:              a pointer to the current element in the
 *                               mapping function
 *      void *t:                 a void pointer, which in this case is expected
 *                               to be a pointer to a UArray2_T struct
 *
 * Return: void.
 *
 * Expects
 *      TBD
 * Notes:
 *      TBD
 ************************/
void apply_2D90(int applyCol, int applyRow, UArray2_T a, void *elem, void *t) 
{

        struct A2methods_struct *holder = t;
        A2Methods_T methods = holder->methods;
        int height = methods->height(a);
        int new_col = (height - applyRow - 1);
        int new_row = applyCol;

        //Pnm_rgb new_location = methods->at(t, new_col, new_row);
        Pnm_rgb new_location = methods->at(holder->A2, new_col, new_row);
        Pnm_rgb x = elem;
        new_location->red = x->red;
        new_location->green = x->green;
        new_location->blue = x->blue;
}

/********** apply_2D180 ********
 *
 * An apply function to be called by the row-major or col-major mapping
 * function that rotates the given UArray by 180 degrees clockwise.
 *
 * Parameters:
 *      int applyCol:            name of the file to be opened
 *      int applyCol:            the mode for opening the file
 *      UArray2_T a:             a pointer to a UArray2_T struct.
 *      void *elem:              a pointer to the current element in the
 *                               mapping function
 *      void *t:                 a void pointer, which in this case is expected
 *                               to be a pointer to a UArray2_T struct
 *
 * Return: void.
 *
 * Expects
 *      TBD
 * Notes:
 *      TBD
 ************************/
void apply_2D180(int applyCol, int applyRow, UArray2_T a, void *elem, void *t) 
{
        struct A2methods_struct *holder = t;
        A2Methods_T methods = holder->methods;
        int height = methods->height(a);
        int width = methods->width(a);
        int new_col = (width - applyCol - 1);
        int new_row = (height - applyRow - 1);

        Pnm_rgb new_location = methods->at(holder->A2, new_col, new_row);
        Pnm_rgb x = elem;
        new_location->red = x->red;
        new_location->green = x->green;
        new_location->blue = x->blue;
}

void apply_row_270(int applyCol, int applyRow, UArray2_T a, void *elem, void *t) 
{
        struct A2methods_struct *holder = t;
        A2Methods_T methods = holder->methods;
        int height = methods->height(a);
        int width = methods->width(a);
        int new_col = (width - applyCol - 1);
        int new_row = (height - applyRow - 1);

        int new270_col = (height - new_row - 1);
        int new270_row = new_col;

        Pnm_rgb new_location = methods->at(holder->A2, new270_col, new270_row);
        Pnm_rgb x = elem;
        new_location->red = x->red;
        new_location->green = x->green;
        new_location->blue = x->blue;
}