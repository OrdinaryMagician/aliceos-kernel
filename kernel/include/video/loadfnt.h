/*
	loadfnt.c : Font loading.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef LOADFNT_H
#define LOADFNT_H
#include <sys/types.h>
/* struct for a font */
typedef struct
{
	Uint8 *data;
	Uint8 w;
	Uint8 h;
	Uint8 flags;
	Uint8 format;
	Uint32 limit;
} fnt_t;

/* load a font from the ramdisk */
Uint8 loadfnt( fnt_t *dest, char *fname );
#endif
