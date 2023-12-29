#ifndef UARRAY2B_INCLUDED
#define UARRAY2B_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "uarray.h"
#include "uarray2.h"

#define T UArray2b_T
typedef struct T *T;

UArray2b_T UArray2b_new(int width, int height, int size, int blocksize);
UArray2b_T UArray2b_new_64K_block(int width, int height, int size);
int UArray2b_width(UArray2b_T array);
int UArray2b_height(UArray2b_T array);
void UArray2b_free(UArray2b_T *UArray2b_T);
int UArray2b_size(UArray2b_T array);
int UArray2b_blocksize(UArray2b_T array);
void* UArray2b_at(UArray2b_T array, int column, int row);
void UArray2b_map(UArray2b_T array, void apply(int applyCol, int applyRow, UArray2b_T a, void *elem, void *closure), void *cl);


#undef T
#endif