/*
	string.c : Memory and string functions.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include "string.h"
#include "screen.h"

Uint8 *memcpy( Uint8 *dest, const Uint8 *src, Uint32 count )
{
	Uint32 i;
	for ( i=0; i<count; i++ )
		dest[i] = src[i];
	return dest;
}

Uint16 *memcpyw( Uint16 *dest, const Uint16 *src, Uint32 count )
{
	Uint32 i;
	for ( i=0; i<count; i++ )
		dest[i] = src[i];
	return dest;
}

Uint8 *memset( Uint8 *dest, Uint8 val, Uint32 count )
{
	Uint32 i;
	for ( i=0; i<count; i++ )
		dest[i] = val;
	return dest;
}

Uint16 *memsetw( Uint16 *dest, Uint16 val, Uint32 count )
{
	Uint32 i;
	for ( i=0; i<count; i++ )
		dest[i] = val;
	return dest;
}

Uint32 strlen( const char *str )
{
	Uint32 i = 0;
	while ( *str++ )
		i++;
	return i;
}

char *strcpy( char *dest, const char *src )
{
	if ( !(*src) )
	{
		*dest = 0;
		return dest;
	}
	while ( *src )
		*dest++ = *src++;
	return dest;
}

char *strcat( char *dest, const char *src )
{
	while ( *dest )
		*dest = *dest++;
	if ( !(*src) )
	{
		*dest = 0;
		return dest;
	}
	while ( *src )
		*dest++ = *src++;
	return dest;
}

Sint8 strcmp( const char *s1, const char *s2 )
{
	Uint32 i = 0;
	while ( s1[i] && s2[i] )
	{
		if ( s1[i] != s2[i] )
			return 1;
		i++;
	}
	if ( (!s1[i] && s2[i]) )
		return -1;
	if ( (s1[i] && !s2[i]) )
		return 1;
	return 0;
}
