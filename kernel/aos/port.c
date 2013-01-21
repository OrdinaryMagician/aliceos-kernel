/*
	port.c : I/O port functions.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <port.h>

/* read/write bytes */

Uint8 inport_b( Uint16 port )
{
	Uint8 ret;
	asm volatile("inb %1, %0":"=a"(ret):"dN"(port));
	return ret;
}

void outport_b( Uint16 port, Uint8 data )
{
	asm volatile("outb %1, %0"::"dN"(port),"a"(data));
}

/* read/write words */

Uint16 inport_w( Uint16 port )
{
	Uint16 ret;
	asm volatile("inw %1, %0":"=a"(ret):"dN"(port));
	return ret;
}

void outport_w( Uint16 port, Uint16 data )
{
	asm volatile("outw %1, %0"::"dN"(port),"a"(data));
}

/* read/write "longs" */

Uint32 inport_l( Uint16 port )
{
	Uint32 ret;
	asm volatile("inl %%dx, %%eax":"=a"(ret):"dN"(port));
	return ret;
}

void outport_l( Uint16 port, Uint32 data )
{
	asm volatile("outl %%eax, %%dx"::"dN"(port),"a"(data));
}

/* read/write multiple words */

void inport_wm( Uint16 port, Uint8 *data, Uint32 size )
{
	asm volatile("rep insw":"+D"(data),"+c"(size):"d"(port):"memory");
}

void outport_wm( Uint16 port, Uint8 *data, Uint32 size )
{
	asm volatile("rep outsw":"+D"(data),"+c"(size):"d"(port));
}
