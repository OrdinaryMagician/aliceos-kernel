/*
	port.c : Basic I/O bus port communications.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include "port.h"

void outb( Uint16 port, Uint8 val )
{
	__asm__ __volatile__ ("outb %1, %0" : : "dN" (port), "a" (val));
}

Uint8 inb( Uint16 port )
{
	Uint8 ret;
	__asm__ __volatile__ ("inb %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}

Uint16 inw( Uint16 port )
{
	Uint16 ret;
	__asm__ __volatile__ ("inw %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}
