#ifndef STRING_H
#define STRING_H
#include "types.h"
Uint8 *memcpy( Uint8 *dest, const Uint8 *src, Uint32 count );
Uint16 *memcpyw( Uint16 *dest, const Uint16 *src, Uint32 count );
Uint8 *memset( Uint8 *dest, Uint8 val, Uint32 count );
Uint16 *memsetw( Uint16 *dest, Uint16 val, Uint32 count );
Uint32 strlen( const char *str );
char *strcpy( char *dest, const char *src );
Sint8 strcmp( const char *s1, const char *s2 );
#endif
