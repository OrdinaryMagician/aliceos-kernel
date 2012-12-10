/*
	port.c : Basic I/O bus port communications.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include "port.h"

void outb( unsigned short port, unsigned char val )
{
	asm volatile("outb %1, %0" : : "dN" (port), "a" (val));
}

unsigned char inb( unsigned short port )
{
	unsigned char ret;
	asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
}

unsigned short inw( unsigned short port )
{
	unsigned short ret;
	asm volatile("inw %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}
