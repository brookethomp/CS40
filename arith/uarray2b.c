
/******************************************************************************
 *
 *                     uarray.c
 *
 *     Assignment: locality
 *     Authors:  Richard Geoghegan (rgeogh01) and Sydney Thompson (sthomp12)
 *     Date:     9 Oct 2023
 *
 *     Summary:
 *     uarray2b.c contains the implementation of the UArray2b_T functionality to
 *     represent a two dimensional unboxed array of any data type. The 
 *     array is organized in blocks. The implementation uses an UArray2 to
 *     represent each block. Each index of contains a UArray to hold the
 *     elements to be stored withinthe block.
 *
 *****************************************************************************/

#include "uarray2b.h"
#include <uarray.h>
#include "uarray2.h"
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_BLOCKSIZE 64 * 1024

#define T UArray2b_T

/* Urray2b_T struct
 * int width: the total number of elements wide being stored in the UArray2b
 * int height: the total number of elements tall being stored in the UArray2b
 * int block_height: number of rows of blocks in the UArray2b
 * int block_width: number of columns of blocks in the UArray2b
 * int size: the size of each element being stored in the UArray2b
 * int blocksize: the amount of elements being stored in one row in the block
 * UArray2_T data: A UArray2 to store the UArray2b in blocked form
 */
struct T {
        int width;
        int height;
        int block_height;
        int block_width;
        int size;
        int blocksize;
        UArray2_T data;
};

/* UArray2b_new
 * Creates a new UArray2b instance which represents a 2D unboxed array with
 * provided number of rows and columns in which the elements are stored in a
 * block with the blocksize being the length of one edge of the block
 * Inputs:
 *      int width: the number of rows that the array will contain
 *      int height: the number of columns that the array will contain
 *      int size: the size, in bytes, of each element that the array will
 *                store
 *      int blocksize: the amount of elements per row in each block
 * Returns: A new instance of the UArray2b_T struct, initialized according to
 *          the given inputs
 * Expects: rows and cols to be zero or greater and block size and size to be 
 *          greater than zero
 * Notes:
 *      New heap memory allocated to reserve space for row x columns elements
 *      to be stored in the array
 *      Will CRE if memory allocation fails
 *      Will CRE if width and height are less than zero
 *      Will CRE if blocksize and size are not positive integers
 */
extern T UArray2b_new (int width, int height, int size, int blocksize) {
        assert(width >= 0);
        assert(height >= 0);
        assert(size > 0);
        assert(blocksize > 0);

        /*use a ceiling function for height and width divided by blocksize*/
        int num_blocks_width  = ((width + blocksize) - 1)/blocksize;
        int num_blocks_height = ((height + blocksize) - 1)/blocksize;

        T array2b = malloc(sizeof(*array2b));
        assert(array2b != NULL);

        array2b->height       = height;
        array2b->width        = width;
        array2b->size         = size;
        array2b->blocksize    = blocksize;
        array2b->block_height = num_blocks_height;
        array2b->block_width  = num_blocks_width;

        array2b->data = UArray2_new(num_blocks_width, num_blocks_height,
                                         sizeof(UArray_T));
        
        /*for each block, create a UArray to store the elements*/
        for(int col = 0; col < num_blocks_width; col++) {
                for(int row = 0; row < num_blocks_height; row++) {
                        UArray_T *block = UArray2_at(array2b->data, col, row);
                        *block = UArray_new(blocksize * blocksize, size);
                }
        }
        return array2b;
}

/* UArray2b_new
 * Creates a new UArray2b instance which represents a 2D unboxed array with
 * provided number of rows and columns in which the elements are stored in a
 * blocks with each block size the most elements that fit within a 64kb block
 * Inputs:
 *      int width: the number of rows that the array will contain
 *      int height: the number of columns that the array will contain
 *      int size: the size, in bytes, of each element that the array will
 *                store
 * Returns: A new instance of the UArray2b_T struct, initialized according to
 *          the given inputs
 * Expects: rows and cols to be zero or greater and block size and size to be 
 *          greater than zero
 * Notes:
 *      New heap memory allocated to reserve space for row x columns elements
 *      to be stored in the array
 *      Will CRE if memory allocation fails
 *      Will CRE if width and height are less than zero
 *      Will CRE if ize is not positive integer
 */
extern T UArray2b_new_64K_block(int width, int height, int size) {
        assert(width >= 0);
        assert(height >= 0);
        assert(size > 0);

        /*get the max blocksize */
        int blocksize = (sqrt(MAX_BLOCKSIZE))/size;
        if(size > MAX_BLOCKSIZE) {
                blocksize = 1;
        }

        return UArray2b_new(width, height, size, blocksize);
}

/* UArray2b_free
 * Frees heap memory from the UArray2b_T instance pointed to by the input
 * Inputs:
 *      UArray2b_T *array: A pointer to the array to free
 * Returns: None
 * Expects: The pointer is nonnull and the array2b in which is point to is 
 *          nonnull
 * Notes:
 *      Heap memory freed for the UArray2b_T struct and all of the elements that
 *      it stores. Array data will no longer be accessible
 *      CRE if array is null or if the UArray2b it points to is null
 */
extern void UArray2b_free (T *array2b) {
        assert(*array2b != NULL);
        assert(array2b != NULL);

        /*free the elements within each block*/
        for(int i = 0; i < (*array2b)->block_width; i++) {
                for(int j = 0; j < (*array2b)->block_height; j++) {
                        UArray_T *block = UArray2_at((*array2b)->data, i, j);
                        UArray_free(block);
                }
        }

        UArray2_free((&(*array2b)->data));
        free(*array2b);
}

/* UArray2b_width
 * Gets the correct width (number of columns) of the UArray2b
 * Inputs: Pointer to a UArray2b
 * Expects: UArray2 to be a non-null array
 * Notes: CRE if array is null
 */
extern int UArray2b_width (T array2b) {
        assert(array2b != NULL);
        return array2b->width;
}

/* UArray2b_height
 * Gets the correct height (number of row) of the UArray2b
 * Inputs: Pointer to a UArray2b
 * Expects: UArray2 to be a non-null array
 * Notes: CRE if array is null
 */
extern int UArray2b_height (T array2b) {
        assert(array2b!= NULL);
        return array2b->height;
        
}

/* UArray2b_size
 * Gets the size of each element in the UArray2b
 * Inputs: Pointer to a UArray2b
 * Expects: UArray2b to be a non-null array
 * Returns: the size of each element
 * Notes: CRE if array is null
 */
extern int UArray2b_size (T array2b) {
        assert(array2b != NULL);
        return array2b->size;
}

/* UArray2b_blocksize
 * Gets the blocksize of the UArray2b
 * Inputs: Pointer to a UArray2b
 * Expects: UArray2b to be a non-null array
 * Returns: the blocksize of the UArray2b
 * Notes: CRE if array is null
 */
extern int UArray2b_blocksize(T array2b) {
        assert(array2b!= NULL);
        return array2b->blocksize;
}

/* UArray2b_at
 * Returns the value of the element at the given row and col of the UArray2b
 * Inputs:
 *      T array2b: the UArray2b to get the element from
 *      int row: the row to index for the wanted element
 *      int col: the col to index for the wanted element
 * Returns: The value of the element at the given row and col
 * Expects: UArray2 given is non-null
            row and col be non-negative integers with row less than the number
            of rows in the UArray2 and col less than the number of cols in the
            UArray2
 * Notes:   CRE if row or col is greater than the respective width and
 *          height
 *          CRE if array passed is null
 */
extern void *UArray2b_at(T array2b, int column, int row) {
        assert(array2b != NULL);
        assert((column < array2b->width) && (column >= 0));
        assert((row < array2b->height) && (row >= 0));
        
        int block_col = column/array2b->blocksize;
        int block_row = row/array2b->blocksize;

        /*first get the correct block*/
        UArray_T *block = UArray2_at(array2b->data, block_col,
                                    block_row);

        /*access the index within the block*/
        int index = array2b->blocksize * (row % array2b->blocksize) +
                                         (column % array2b->blocksize);
        return (UArray_at(*block, index));
}

/* UArray2_map
 * Applies the given function to each element of the UArray2b in one block
 * before moving to another block
 * Inputs:
 *      UArray2_T array: the array to apply the function to
 *      void apply(...): a function pointer to the function that will be applied
 *                       to each of the elements in the array
 *      void *cl: a pointer to an accumulator variable that can track something
 *                about the elements as they are being mapped
 * Returns: None
 * Expects: array to be a pointer to a nonnull UArray2b_T struct
 * Notes:
 *      Will CRE if the array is null
 */
extern void UArray2b_map(T array2b,
void apply(int col, int row, T array2b,
void *elem, void *cl),
void *cl) { 
        assert(array2b != NULL);
        int arrayCol, arrayRow;

        for(int row = 0; row < array2b->block_height; row++) {
                for(int col = 0; col < array2b->block_width; col++) {
                        UArray_T *block = UArray2_at(array2b->data, col, row);
                        for (int k = 0; k < UArray_length(*block); k++) {
                                /*get the row and col within the whole image
                                  not within a block*/
                                arrayCol = (col * array2b->blocksize) +
                                                 (k % array2b->blocksize);
                                arrayRow = (row * array2b->blocksize) +
                                                 (k / array2b->blocksize);

                                /* check if index is within image bounds */
                                if ((arrayCol < array2b->width) &&
                                    (arrayRow < array2b->height)) {
                                        void *elem = UArray_at(*block, k);
                                        apply(arrayCol, arrayRow,
                                                 array2b, elem, cl);
                                }
                        }
                }       
        }
}