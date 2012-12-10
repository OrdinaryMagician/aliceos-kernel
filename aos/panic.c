#include "panic.h"
#include "screen.h"

extern void panic( const char *message, const char *file, Uint32 line )
{
	puts("PANIC(");
	puts(message);
	puts(") at ");
	puts(file);
	puts(":");
	putu(line);
	putc('\n');
	__asm__ __volatile__ ("cli");
	__asm__ __volatile__ ("hlt");
}

extern void panic_assert( const char *file, Uint32 line, const char *desc )
{
	puts("ASSERTION-FAILED(");
	puts(desc);
	puts(") at ");
	puts(file);
	puts(":");
	putu(line);
	putc('\n');
	__asm__ __volatile__ ("cli");
	__asm__ __volatile__ ("hlt");
}
