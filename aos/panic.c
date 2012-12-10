#include "panic.h"
#include "screen.h"

extern void panic( const char *message, const char *file, Uint32 line )
{
	__asm__ __volatile__ ("cli");
	puts("PANIC(");
	puts(message);
	puts(") at ");
	puts(file);
	puts(":");
	putu(line);
	putc('\n');
	for(;;);
}

extern void panic_assert( const char *file, Uint32 line, const char *desc )
{
	__asm__ __volatile__ ("cli");
	puts("ASSERTION-FAILED(");
	puts(desc);
	puts(") at ");
	puts(file);
	puts(":");
	putu(line);
	putc('\n');
	for(;;);
}
