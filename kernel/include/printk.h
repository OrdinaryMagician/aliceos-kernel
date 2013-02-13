/*
	printk.h : The almighty debug function.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef PRINTK_H
#define PRINTK_H
#include <sys/types.h>

/* printing to a serial device, for debug purposes */
void printk( Uint16 dev, char *s, ... );

#endif
