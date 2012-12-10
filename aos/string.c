/*
	string.c : Memory and string functions.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include "string.h"

extern Uint8 *memcpy( Uint8 *dest, const Uint8 *src, int count )
{
	int i;
	for ( i=0; i<count; i++ )
		dest[i] = src[i];
	return dest;
}

extern Uint16 *memcpyw( Uint16 *dest, const Uint16 *src, int count )
{
	int i;
	for ( i=0; i<count; i++ )
		dest[i] = src[i];
	return dest;
}

extern Uint8 *memset( Uint8 *dest, Uint8 val, int count )
{
	int i;
	for ( i=0; i<count; i++ )
		dest[i] = val;
	return dest;
}

extern Uint16 *memsetw( Uint16 *dest, Uint16 val, int count )
{
	int i;
	for ( i=0; i<count; i++ )
		dest[i] = val;
	return dest;
}

extern int strlen( const char *str )
{
	int i = 0;
	while ( str[i] != 0 )
		i++;
	return i;
}
