/*
	vgafont.h : BIOS font patches.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef VGAFONT_H
#define VGAFONT_H
#include <types.h>
/* local copy of bios font */
Uint8 biosfnt[4096];
/* apply patches to the BIOS font, save a local copy */
void vga_initfont( void );
#endif
