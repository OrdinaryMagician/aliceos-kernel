/*
	vgaloadimg.c : Image loading.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef VGALOADIMG_H
#define VGALOADIMG_H
#include <types.h>
/* struct for an image */
typedef struct
{
	Uint8 *data;
	Uint8 *pal;
	Uint16 w;
	Uint16 h;
	Uint8 depth;
} img_t;

/* load an image from the ramdisk */
Uint8 vga_loadimg( img_t *dest, char *fname );
#endif
