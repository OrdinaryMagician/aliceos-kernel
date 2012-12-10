#ifndef PANIC_H
#define PANIC_H
#include "types.h"
#define PANIC(msg) panic(msg,__FILE__,__LINE__);
#define ASSERT(b) ((b) ? (void)0 : panic_assert(__FILE__,__LINE__,#b))
extern void panic( const char *message, const char *file, Uint32 line );
extern void panic_assert( const char *file, Uint32 line, const char *desc );
#endif
