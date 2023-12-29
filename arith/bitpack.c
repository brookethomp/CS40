/*******************************************************************************
 *
 *                     bitpack.c
 *
 *     Assignment: arith
 *     Authors: Brooke Thompson (bthomp05) and Richard Geoghegan (rgeogh01)
 *     Date:     10/20/23
 *
 *     This file provides a program that includes functions that will create 
 *     new signed and unsigned bitpacked word, a bitmask, and
 *     find a segment of an unsigned word.
 *
 ******************************************************************************/
#include "bitpack.h"
#include "assert.h"

Except_T Bitpack_Overflow = { "Overflow packing bits" };

#define MAX_WIDTH 64

/********** Bitpack_createMask ********
 *
 * Creates a bitmask of a given width.
 *
 * Parameters:
 *      unsigned width: Width of the bitmask
 *
 * Return: Unsigned 64 bitmask of certain width
 *
 * Expects
 *      N/A
 * Notes:
 *      N/A
 ************************/
static uint64_t Bitpack_createMask(unsigned width)
{
	return (1ULL << width) - 1;
}

/********** Bitpack_fitss ********
 *
 * Checks to see if the given segment of unsigned bits fits in 
 * the provided width.
 *
 * Parameters:
 *	 	int64_t n: A segment of bits
 *      unsigned width: Width of the bitmask
 *
 * Return: True if the bit fits in the bounds of the given width, False if
 * 		   the bits do not fit the bounds of the given width.
 *
 * Expects:
 *      Width should be less than or equal to the maximum width of 64 bits
 * Notes:
 *      N/A
 ************************/
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
	assert(width <= MAX_WIDTH);
	return n < (1ULL << width);
}

/********** Bitpack_fitss ********
 *
 * Checks to see if the given segment of signed bits fits in 
 * the provided width.
 *
 * Parameters:
 *	 	int64_t n: A segment of bits
 *      unsigned width: Width of the bitmask
 *
 * Return: True if the bit fits in the bounds of the given width, False if
 * 		   the bits do not fit the bounds of the given width.
 *
 * Expects
 *      Width should be less than or equal to the maximum width of 64 bits
 * Notes:
 *      N/A
 ************************/
bool Bitpack_fitss(int64_t n, unsigned width)
{
	assert(width <= MAX_WIDTH);
	int64_t min = -(1LL << (width - 1));
	int64_t max = (1LL << (width - 1)) - 1;
	return n >= min && n <= max;
}

/********** Bitpack_getu ********
 *
 * Gets a segments of unsinged bits out of a word of 64 packed bits.
 *
 * Parameters:
 *	 	uint64_t word: The word bitpack that is being added to
 *		width: Width of the bitmask
 *		unsigned lsb: Width of the least significant bit
 *
 * Return: Return a unsigned segment of bits from a 64 bitpacked word.
 *
 * Expects
 *      Width and the least significant bit should be less than or equal 
 *		to the maximum width of 64 bits.
 * Notes:
 *      N/A
 ************************/
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
	assert(width + lsb <= MAX_WIDTH);

	uint64_t shifted_word = word >> lsb;
	return shifted_word & Bitpack_createMask(width);
}

/********** Bitpack_gets ********
 *
 * Gets a segments of singed bits out of a word of 64 packed bits.
 *
 * Parameters:
 *	 	uint64_t word: The word bitpack that is being added to
 *		width: Width of the bitmask
 *		unsigned lsb: Width of the least significant bit
 *
 * Return: Return a signed segment of bits from a 64 bitpacked word.
 *
 * Expects
 *      Width and the least significant bit should be less than or 
 *		equal to the maximum width of 64 bits.
 * Notes:
 *      N/A
 ************************/
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
	assert(width + lsb <= MAX_WIDTH);
	
	uint64_t left_shifted = word << (MAX_WIDTH - width - lsb);
	int64_t value = (int64_t)left_shifted >> (MAX_WIDTH - width);
    
    	return value;
}

/********** Bitpack_newu ********
 *
 * Adds a new segment of unsigned bits into a word of 64 packed bits.
 *
 * Parameters:
 *	 	uint64_t word: The word bitpack that is being added to
 *		width: Width of the bit segment
 *		unsigned lsb: Width of the least significant bit
 *		uint64_t value: One part of the words as a segment of bits
 *
 * Return: A 64 bitpacked word with an added unsigned bit segment.
 *
 * Expects
 *      Width and the least significant bit should be less than or equal 
 * 		to the maximum width of 64 bits. If it is outside then bounds then it 
 *		raises a bitpack overflow expression.
 * Notes:
 *      N/A
 ************************/
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
							uint64_t value)
{
	assert(width + lsb <= MAX_WIDTH);

	if (Bitpack_fitsu(value, width) != true) {
		RAISE(Bitpack_Overflow);
        	assert(0);
    	}
    
    	uint64_t mask = Bitpack_createMask(width) << lsb;
    	return (word & ~mask) | (value << lsb);
}

/********** Bitpack_news ********
 *
 * Adds a new segment of signed bits into a word of 64 packed bits.
 *
 * Parameters:
 *	 	uint64_t word: The word bitpack that is being added to
 *		width: Width of the bit segment
 *		unsigned lsb: Width of the least significant bit
 *		uint64_t value: One part of the words as a segment of bits
 *
 * Return: A 64 bitpacked word with an added signed bit segment.
 *
 * Expects
 *      Width and the least significant bit should be less than or equal to 
 * 		the maximum width of 64 bits. If it is outside then bounds then it 
 *		raises a bitpack overflow expression.
 * Notes:
 *      N/A
 ************************/
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,
							int64_t value)
{
   	assert(width + lsb <= MAX_WIDTH);
    
    	if (Bitpack_fitss(value, width) != true) {
		RAISE(Bitpack_Overflow);
        	assert(0);
	}

	uint64_t mask = Bitpack_createMask(width) << lsb;
	return (word & ~mask) | ((uint64_t)value << lsb & mask);
}