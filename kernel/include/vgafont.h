/*
	vgafont.h : BIOS font patches.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef VGAFONT_H
#define VGAFONT_H
#include <types.h>
#include <vgatext.h>
/* apply patches to the current font (which is expected to be the BIOS one) */
void vga_patchfont( void );
#endif
