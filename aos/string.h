#ifndef STRING_H
#define STRING_H
#include "types.h"
Uint8 *memcpy( Uint8 *dest, const Uint8 *src, int count );
Uint8 *memset( Uint8 *dest, Uint8 val, int count );
Uint16 *memsetw( Uint16 *dest, Uint16 val, int count );
int strlen( const char *str );
#endif
