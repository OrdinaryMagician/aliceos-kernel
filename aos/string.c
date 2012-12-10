/*
	string.c : Memory and string functions.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include "string.h"

extern unsigned char *memcpy( unsigned char *dest, const unsigned char *src, int count )
{
	int i;
	for ( i=0; i<count; i++ )
		dest[i] = src[i];
	return dest;
}

extern unsigned char *memset( unsigned char *dest, unsigned char val, int count )
{
	int i;
	for ( i=0; i<count; i++ )
		dest[i] = val;
	return dest;
}

extern unsigned short *memsetw( unsigned short *dest, unsigned short val, int count )
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
