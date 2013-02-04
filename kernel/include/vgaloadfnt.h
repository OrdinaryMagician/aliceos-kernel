/*
	vgaloadfnt.c : Font loading.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef VGALOADFNT_H
#define VGALOADFNT_H
#include <types.h>
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
Uint8 vga_loadfnt( fnt_t *dest, char *fname );
#endif
