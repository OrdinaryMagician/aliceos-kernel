/*
	printk.h : The almighty function.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef PRINTK_H
#define PRINTK_H
#include <types.h>
#include <vgatext.h>
#include <va_list.h>
#include <serial.h>

/* kernel's own personal printf */
void printk( char *s, ... );
void printk_s( Uint16 dev, char *s, ... );

#endif
