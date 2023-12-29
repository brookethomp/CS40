/*******************************************************************************
 *
 *                     a2plain.c
 *
 *     Assignment: arith
 *     Authors: Brooke Thompson (bthomp05) and Richard Geoghegan (rgeogh01)
 *     Date:     10/20/23
 *
 *     This file provides a private version of each function in
 *     A2Methods_T that we implement. 
 *
 ******************************************************************************/
#include <string.h>

#include <a2plain.h>
#include "uarray2.h"

/************************************************/
/* Define a private version of each function in */
/* A2Methods_T that we implement.               */
/************************************************/

typedef A2Methods_UArray2 A2;

/* new
 * creates a new UArray2
 * Inputs: int width: the width of the array
 *         int height: the height of the array
 *         int size: the size of each element to be stored
 * Expects: Nothing
 */
static A2Methods_UArray2 new(int width, int height, int size)
{
        return UArray2_new(width, height, size);
}


/* new_with_blocksize
 * creates a new UArray2
 * Inputs: int width: the width of the array
 *         int height: the height of the array
 *         int size: the size of each element to be stored
 * Returns: a new A2Methods_UArray2 using a plain UArray2
 * Expects: Nothing
 * Notes: Ignores the blocksize 
 */
static A2Methods_UArray2 new_with_blocksize(int width, int height, int size,
                                            int blocksize)
{
        (void) blocksize;
        return UArray2_new(width, height, size);
}

/* a2free
 * frees a provided UArray2
 * Inputs: pointer to an array2
 * Returns: nothing
 * Expects: Nothing
 */
static void a2free(A2 * array2)
{
        UArray2_free((UArray2_T *) array2);
}

/* width
 * get the width of an array2
 * Inputs: pointer to an array2
 * Returns: the width of the array2
 * Expects: Nothing
 */
static int width(A2 array2)
{
        return UArray2_width(array2);
}

/* height
 * get the height of an array2
 * Inputs: pointer to an array2
 * Returns: the height of the array2
 * Expects: Nothing
 */
static int height(A2 array2)
{
        return UArray2_height(array2);
}

/* size
 * get the size of each element of an array2
 * Inputs: pointer to an array2
 * Returns: the size of each element of the array2
 * Expects: Nothing
 */
static int size(A2 array2)
{
        return UArray2_size(array2);
}

/* blocksize
 * get the blocksize of an array2
 * Inputs: pointer to an array2
 * Returns: the blocksize of the array2
 * Expects: Nothing
 */
static int blocksize(A2 array2)
{      
        (void) array2;
        return 1;
}

/* at
 * Returns a void pointer to what the array is holding at a given col and row
 * A2 array2: UArray2 containing the objects.
 * int i: Col index
 * int j: Row index
*/
static A2Methods_Object *at(A2 array2, int i, int j)
{
        return UArray2_at(array2, i, j);
}

static void map_row_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_row_major(uarray2, (UArray2_applyfun*)apply, cl);
}

static void map_col_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_col_major(uarray2, (UArray2_applyfun*)apply, cl);
}

struct small_closure {
        A2Methods_smallapplyfun *apply; 
        void                    *cl;
};

static void apply_small(int i, int j, UArray2_T uarray2,
                        void *elem, void *vcl)
{
        struct small_closure *cl = vcl;
        (void)i;
        (void)j;
        (void)uarray2;
        cl->apply(elem, cl->cl);
}

static void small_map_row_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_row_major(a2, apply_small, &mycl);
}

static void small_map_col_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_col_major(a2, apply_small, &mycl);
}

static struct A2Methods_T uarray2_methods_plain_struct = {
        new,
        new_with_blocksize,
        a2free,
        width,
        height,
        size,
        blocksize,
        at,
        map_row_major,        
        map_col_major,                  
        NULL,        // map_block_major
        map_row_major,        
        small_map_row_major,                   
        small_map_col_major,                  
        NULL,           //small_map_block_major
        small_map_row_major
};

// finally the payoff: here is the exported pointer to the struct

A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;
