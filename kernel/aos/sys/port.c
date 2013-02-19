/*
	port.c : I/O port functions.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/port.h>
#include <sys/helpers.h>

/* read/write bytes */

Uint8 inport_b( Uint16 port )
{
	Uint8 ret;
	#ifdef ATT_SYNTAX
	asm volatile("inb %1, %0":"=a"(ret):"dN"(port));
	#else
	asm volatile("in %0, %1":"=a"(ret):"dN"(port));
	#endif
	return ret;
}

void outport_b( Uint16 port, Uint8 data )
{
	#ifdef ATT_SYNTAX
	asm volatile("outb %1, %0"::"dN"(port),"a"(data));
	#else
	asm volatile("out %0, %1"::"dN"(port),"a"(data));
	#endif
}

/* read/write words */

Uint16 inport_w( Uint16 port )
{
	Uint16 ret;
	#ifdef ATT_SYNTAX
	asm volatile("inw %1, %0":"=a"(ret):"dN"(port));
	#else
	asm volatile("in %0, %1":"=a"(ret):"dN"(port));
	#endif
	return ret;
}

void outport_w( Uint16 port, Uint16 data )
{
	#ifdef ATT_SYNTAX
	asm volatile("outw %1, %0"::"dN"(port),"a"(data));
	#else
	asm volatile("out %0, %1"::"dN"(port),"a"(data));
	#endif
}

/* read/write "longs" */

Uint32 inport_l( Uint16 port )
{
	Uint32 ret;
	#ifdef ATT_SYNTAX
	asm volatile("inl %%dx, %%eax":"=a"(ret):"dN"(port));
	#else
	asm volatile("in %%eax, %%dx":"=a"(ret):"dN"(port));
	#endif
	return ret;
}

void outport_l( Uint16 port, Uint32 data )
{
	#ifdef ATT_SYNTAX
	asm volatile("outl %%eax, %%dx"::"dN"(port),"a"(data));
	#else
	asm volatile("out %%dx, %%eax"::"dN"(port),"a"(data));
	#endif
}
