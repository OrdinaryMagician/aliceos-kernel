/*
	memops.h : Basic memory block operations.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef MEMOPS_H
#define MEMOPS_H
#include <types.h>

/* copy byte blocks */
Uint8 *memcpy( Uint8 *dest, const Uint8 *src, Uint32 count );

/* copy word blocks */
Uint16 *memcpyw( Uint16 *dest, const Uint16 *src, Uint32 count );

/* fill an entire memory block with a repeating byte */
Uint8 *memset( Uint8 *dest, Uint8 val, Uint32 count );

/* fill an entire memory block with a repeating word */
Uint16 *memsetw( Uint16 *dest, Uint16 val, Uint32 count );

#endif
