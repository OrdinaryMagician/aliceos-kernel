/*
	memops.c : Basic memory block operations.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <memops.h>

/* copy byte blocks */
Uint8 *memcpy( Uint8 *dest, const Uint8 *src, Uint32 count )
{
	Uint32 i;
	for ( i=0; i<count; i++ )
		dest[i] = src[i];
	return dest;
}

/* copy word blocks */
Uint16 *memcpyw( Uint16 *dest, const Uint16 *src, Uint32 count )
{
	Uint32 i;
	for ( i=0; i<count; i++ )
		dest[i] = src[i];
	return dest;
}

/* fill an entire memory block with a repeating byte */
Uint8 *memset( Uint8 *dest, Uint8 val, Uint32 count )
{
	Uint32 i;
	for ( i=0; i<count; i++ )
		dest[i] = val;
	return dest;
}

/* fill an entire memory block with a repeating word */
Uint16 *memsetw( Uint16 *dest, Uint16 val, Uint32 count )
{
	Uint32 i;
	for ( i=0; i<count; i++ )
		dest[i] = val;
	return dest;
}
