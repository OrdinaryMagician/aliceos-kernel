/*
	memops.c : Basic memory block operations.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <memops.h>
/* copy byte blocks */
void *memcpy( void *dest, const void *src, uint32_t count )
{
	uint8_t *_dest = (uint8_t*)dest, *_src = (uint8_t*)src;
	while ( count-- )
		*(_dest++) = *(_src++);
	return dest;
}
/* copy byte blocks checking for possible overlap */
void *memmove( void *dest, const void *src, uint32_t count )
{
	uint8_t *_dest = (uint8_t*)dest, *_src = (uint8_t*)src;
	if ( _dest == _src )
		return dest;
	if ( (_src+count <= _dest) || (_dest+count <= _src) )
	{
		while ( count-- )
			*(_dest++) = *(_src++);
		return dest;
	}
	if ( _dest > _src )
	{
		_dest += count-1;
		_src += count-1;
		while ( count-- )
			*(_dest--) = *(_src--);
		return dest;
	}
	while ( count-- )
		*(_dest++) = *(_src++);
	return dest;
}
/* copy word blocks */
uint16_t *memcpyw( uint16_t *dest, const uint16_t *src, uint32_t count )
{
	uint16_t *_dest = dest, *_src = (uint16_t*)src;
	while ( count-- )
		*(_dest++) = *(_src++);
	return dest;
}
/* copy word blocks checking for possible overlap */
uint16_t *memmovew( uint16_t *dest, const uint16_t *src, uint32_t count )
{
	uint16_t *_dest = dest, *_src = (uint16_t*)src;
	if ( _dest == _src )
		return dest;
	if ( (_src+count <= _dest) || (_dest+count <= _src) )
	{
		while ( count-- )
			*(_dest++) = *(_src++);
		return dest;
	}
	if ( _dest > _src )
	{
		_dest += count-1;
		_src += count-1;
		while ( count-- )
			*(_dest--) = *(_src--);
		return dest;
	}
	while ( count-- )
		*(_dest++) = *(_src++);
	return dest;
}
/* fill an entire memory block with a repeating byte */
void *memset( void *dest, uint8_t val, uint32_t count )
{
	uint8_t *_dest = (uint8_t*)dest;
	while ( count-- )
		*(_dest++) = val;
	return dest;
}
/* fill an entire memory block with a repeating word */
uint16_t *memsetw( uint16_t *dest, uint16_t val, uint32_t count )
{
	uint16_t *_dest = dest;
	while ( count-- )
		*(_dest++) = val;
	return dest;
}
/* compare memory blocks, returns 0 if equal, 1 if different */
uint8_t memcmp( void *a, void *b, uint32_t count )
{
	uint8_t *_a = (uint8_t*)a, *_b = (uint8_t*)b;
	while ( count-- )
	{
		if ( *(_a++) != *(_b++) )
			return 1;
	}
	return 0;
}
/* compare memory blocks by words, returns 0 if equal, 1 if different */
uint8_t memcmpw( uint16_t *a, uint16_t *b, uint32_t count )
{
	uint16_t *_a = (uint16_t*)a, *_b = (uint16_t*)b;
	while ( count-- )
	{
		if ( *(_a++) != *(_b++) )
			return 1;
	}
	return 0;
}
/* reverse a memory block */
void *memrev( void *dest, uint32_t count )
{
	uint8_t *a = (uint8_t*)dest;
	uint8_t *b = a+count-1;
	while ( a<b )
	{
		*a ^= *b;
		*b ^= *a;
		*(a++) ^= *(b--);
	}
	return dest;
}
/* reverse a memory block by words */
uint16_t *memrevw( uint16_t *dest, uint32_t count )
{
	uint16_t *a = (uint16_t*)dest;
	uint16_t *b = a+count-1;
	while ( a<b )
	{
		*a ^= *b;
		*b ^= *a;
		*(a++) ^= *(b--);
	}
	return dest;
}
/* find first occurence of a specific byte in a memory block */
void *memb( void *in, uint8_t b, uint32_t count )
{
	uint8_t *_in = (uint8_t*)in;
	while ( count-- )
		if ( *(_in++) == b )
			return (_in-1);
	return NULL;
}
/* find first occurence of a specific word in a memory block */
uint16_t *memw( uint16_t *in, uint16_t w, uint32_t count )
{
	while ( count-- )
		if ( *(in++) == w )
			return (in-1);
	return NULL;
}
