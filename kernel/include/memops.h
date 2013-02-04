/*
	memops.h : Basic memory block operations.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef MEMOPS_H
#define MEMOPS_H
#include <types.h>

/* copy byte blocks */
Uint8 *memcpy( Uint8 *dest, const Uint8 *src, Uint32 count );

/* copy byte blocks checking for possible overlap */
Uint8 *memmove( Uint8 *dest, const Uint8 *src, Uint32 count );

/* copy word blocks */
Uint16 *memcpyw( Uint16 *dest, const Uint16 *src, Uint32 count );

/* copy word blocks checking for possible overlap */
Uint16 *memmovew( Uint16 *dest, const Uint16 *src, Uint32 count );

/* fill an entire memory block with a repeating byte */
Uint8 *memset( Uint8 *dest, Uint8 val, Uint32 count );

/* fill an entire memory block with a repeating word */
Uint16 *memsetw( Uint16 *dest, Uint16 val, Uint32 count );

/* compare memory blocks, returns 0 if equal, 1 if different */
Uint8 memcmp( Uint8 *a, Uint8 *b, Uint32 count );

/* compare memory blocks by words, returns 0 if equal, 1 if different */
Uint8 memcmpw( Uint16 *a, Uint16 *b, Uint32 count );

/* reverse a memory block */
Uint8 *memrev( Uint8 *dest, Uint32 count );

/* reverse a memory block by words */
Uint16 *memrevw( Uint16 *dest, Uint32 count );

/* find first occurence of a specific byte in a memory block */
/* return 0 if not found, or the offset plus one if found */
Uint32 memb( Uint8 *in, Uint8 b, Uint32 count );

/* find first occurence of a specific word in a memory block */
/* return 0 if not found, or the offset plus one if found */
Uint32 memw( Uint16 *in, Uint16 w, Uint32 count );

#endif
