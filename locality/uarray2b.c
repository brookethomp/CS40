/*******************************************************************************
 *
 *                     uarray2b.c
 *
 *     Assignment: locality
 *     Authors: Brooke Thompson (bthomp05) and Ian Ryan (iryan01)
 *     Date:     10/10/23
 *
 *     This file provides a program that...
 *
 ******************************************************************************/

#include "uarray2b.h"
#include "uarray2.h"
#include "uarray.h"
#include "math.h"
#include "cputiming.h"
#include "mem.h"
#include "assert.h"
#include "stdbool.h"
#include <stdlib.h>

struct UArray2b_T {
        int width, height;
        int size;
        int blocksize;
        UArray2_T blocks;
};

/**********UArray2b_new********
 *
 * Calls readaline and separates number and non-number characters into 
 * different character arrays. Passes these arrays to sequenceHashTable to be 
 * added into the Hanson table.  
 * 
 * Inputs:
 *		char **datapp: points to array of characters read by readaline
 *      FILE* inputfd: points to file from which we are reading.
 * Return: nothing 
 * Effects: 
 *      Calls 
 * Expects
 *      int *scores to be nonnull
 * Notes:
 *		value of scoresUnderLimit is set to the number of scores under the limit
 *		May CRE if malloc fails.
 *      Will CRE if *scores 
 * *******************************/
extern UArray2b_T UArray2b_new(int width, int height, int size, int blocksize)
{
        UArray2b_T array;
        NEW(array);
        array->width  = width;
        array->height = height;
        array->size   = size;
        array->blocksize = blocksize;

        double ceilheight = height;
        double ceilwidth = width;
        double ceilblock = blocksize;
        double blockheight = ceil(ceilheight/ceilblock);
        double blockwidth = ceil(ceilwidth/ceilblock);

        array->blocks = UArray2_new(blockwidth, blockheight, sizeof(UArray_T));
        for (int j = 0; j < blockheight; j++) {
                for (int i = 0; i < blockwidth; i++){
                    UArray_T *cells = UArray2_at(array->blocks, i, j);
                    *cells = UArray_new((blocksize * blocksize), size);
                }
        }
        return array;
}

extern UArray2b_T UArray2b_new_64K_block(int width, int height, int size)
{
    const int cellsMax_64KB = 64 * 1024 / size;

    int max_blocksize = 1;
    while (max_blocksize * max_blocksize * size <= cellsMax_64KB) {
        max_blocksize++;
    }

    if (max_blocksize == 1) {
        return UArray2b_new(width, height, size, 1);
    } else {
        return UArray2b_new(width, height, size, max_blocksize);
    }
}

extern void UArray2b_free(UArray2b_T *array2b)
{
        assert(array2b && *array2b);
        double bsize = (*array2b)->blocksize;
        double blockheight = ceil(UArray2b_height(*array2b)/bsize);
        double blockwidth = ceil(UArray2b_width(*array2b)/bsize);
        for (int i = 0; i < blockwidth; i++) {
                for (int j = 0; j < blockheight; j++){
                    UArray_T *cells = UArray2_at((*array2b)->blocks, i, j);
                    UArray_free(cells);
                }
        }
        UArray2_free(&(*array2b)->blocks);
        FREE(*array2b);
}

extern void *UArray2b_at(UArray2b_T array2b, int i, int j)
{
        assert(array2b);
        int block_col = i/array2b->blocksize;
        int block_row = j/array2b->blocksize;
        UArray_T *cells = UArray2_at(array2b->blocks, block_col, block_row);
        int cell_index = array2b->blocksize * (i % array2b->blocksize)
                       + (j % array2b->blocksize);
        void *value = UArray_at(*cells, cell_index);
        return value;
}

extern int UArray2b_height(UArray2b_T array2b)
{
        assert(array2b);
        return array2b->height;
}

extern int UArray2b_width(UArray2b_T array2b)
{
        assert(array2b);
        return array2b->width;
}

extern int UArray2b_size(UArray2b_T array2b)
{
        assert(array2b);
        return array2b->size;
}

extern int UArray2b_blocksize(UArray2b_T array2b){
        assert(array2b);
        return array2b->blocksize;
}

extern void UArray2b_map(UArray2b_T array2b, 
                           void apply(int applyCol, int applyRow, UArray2b_T array2b, 
                                      void *elem, void *closure), 
                           void *cl)
{
        assert(array2b);
        int bsize = array2b->blocksize;
        int row = 0;
        int col = 0;
        int colRange = bsize;
        int rowRange = bsize;

        double ceilheight = array2b->height;
        double ceilwidth = array2b->width;
        double ceilblock = UArray2b_blocksize(array2b);

        double blockwidth = ceil(ceilwidth/ceilblock);
        double blockheight = ceil(ceilheight/ceilblock);

        int col_max = blockwidth * bsize;
        int row_max = blockheight * bsize;
        
        int extraRows = array2b->height % array2b->blocksize;
        int extraCols = array2b->width % array2b->blocksize;
        if (extraRows == 0) {
                extraRows = array2b->blocksize;
        }
        if (extraCols == 0) {
                extraCols = array2b->blocksize;
        }

        for (int i = 0; i < blockheight; i++) {
                for (int j = 0; j < blockwidth; j++) {
                        if (i == (blockheight-1)) {
                                        rowRange = row + extraRows;
                        }
                        for (int r = row; r < rowRange; r++) {
                                if (j == (blockwidth-1)) {
                                        colRange = col + extraCols;
                                }
                                for (int c = col; c < colRange; c++) {
                                        apply(c, r, array2b, UArray2b_at(array2b, c, r), cl);

                                }
                        }
                        if ((rowRange == row_max) && (colRange == col_max)) {
                                return;
                        }
                        col = col + bsize;
                        colRange = colRange + bsize;
                }
                col = 0;
                colRange = bsize;
                row = row + bsize;
                rowRange = rowRange + bsize;
        }
           
}