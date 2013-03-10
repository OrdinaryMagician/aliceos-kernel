/*
	memops.c : Basic memory block operations.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <memops.h>
/* copy byte blocks */
uint8_t *memcpy( uint8_t *dest, const uint8_t *src, uint32_t count )
{
	uint32_t i;
	for ( i=0; i<count; i++ )
		dest[i] = src[i];
	return dest;
}
/* copy byte blocks checking for possible overlap */
uint8_t *memmove( uint8_t *dest, const uint8_t *src, uint32_t count )
{
	uint32_t i;
	if ( dest == src )
		return dest;
	if ( (src+count <= dest) || (dest+count <= src) )
	{
		for ( i=0; i<count; i++ )
			dest[i] = src[i];
		return dest;
	}
	if ( dest > src )
	{
		i = count;
		while ( i-- )
			dest[i] = src[i];
		return dest;
	}
	for ( i=0; i<count; i++ )
		dest[i] = src[i];
	return dest;
}
/* copy word blocks */
uint16_t *memcpyw( uint16_t *dest, const uint16_t *src, uint32_t count )
{
	uint32_t i;
	for ( i=0; i<count; i++ )
		dest[i] = src[i];
	return dest;
}
/* copy word blocks checking for possible overlap */
uint16_t *memmovew( uint16_t *dest, const uint16_t *src, uint32_t count )
{
	uint32_t i;
	if ( dest == src )
		return dest;
	if ( (src+count <= dest) || (dest+count <= src) )
	{
		for ( i=0; i<count; i++ )
			dest[i] = src[i];
		return dest;
	}
	if ( dest > src )
	{
		i = count;
		while ( i-- )
			dest[i] = src[i];
		return dest;
	}
	for ( i=0; i<count; i++ )
		dest[i] = src[i];
	return dest;
}
/* fill an entire memory block with a repeating byte */
uint8_t *memset( uint8_t *dest, uint8_t val, uint32_t count )
{
	uint32_t i = 0;
	while ( i < count )
		dest[i++] = val;
	return dest;
}
/* fill an entire memory block with a repeating word */
uint16_t *memsetw( uint16_t *dest, uint16_t val, uint32_t count )
{
	uint32_t i = 0;
	while ( i < count )
		dest[i++] = val;
	return dest;
}
/* compare memory blocks, returns 0 if equal, 1 if different */
uint8_t memcmp( uint8_t *a, uint8_t *b, uint32_t count )
{
	uint32_t i = 0;
	while ( i < count )
	{
		if ( a[i] != b[i] )
			return 1;
		i++;
	}
	return 0;
}
/* compare memory blocks by words, returns 0 if equal, 1 if different */
uint8_t memcmpw( uint16_t *a, uint16_t *b, uint32_t count )
{
	uint32_t i = 0;
	while ( i < count )
	{
		if ( a[i] != b[i] )
			return 1;
		i++;
	}
	return 0;
}
/* reverse a memory block */
uint8_t *memrev( uint8_t *dest, uint32_t count )
{
	uint32_t i = 0;
	uint32_t j = count-1;
	uint8_t tmp;
	while ( i < j )
	{
		tmp = dest[i];
		dest[i++] = dest[j];
		dest[j--] = tmp;
	}
	return dest;
}
/* reverse a memory block by words */
uint16_t *memrevw( uint16_t *dest, uint32_t count )
{
	uint32_t i = 0;
	uint32_t j = count-1;
	uint16_t tmp;
	while ( i < j )
	{
		tmp = dest[i];
		dest[i++] = dest[j];
		dest[j--] = tmp;
	}
	return dest;
}
/* find first occurence of a specific byte in a memory block */
/* return 0 if not found, or the offset plus one if found */
uint32_t memb( uint8_t *in, uint8_t b, uint32_t count )
{
	uint32_t i = 0;
	while ( i < count )
	{
		if ( in[i++] == b )
			return i;
	}
	return 0;
}
/* find first occurence of a specific word in a memory block */
/* return 0 if not found, or the offset plus one if found */
uint32_t memw( uint16_t *in, uint16_t w, uint32_t count )
{
	uint32_t i = 0;
	while ( i < count )
	{
		if ( in[i++] == w )
			return i;
	}
	return 0;
}
