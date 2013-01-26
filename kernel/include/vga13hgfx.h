/*
	vga13hgfx.h : mode 13h graphics.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef VGA13HGFX_H
#define VGA13HGFX_H
#include <types.h>
#include <vgareg.h>
#include <vgafont.h>

/* draw a pixel on screen */
void vga_13hputpixel( Uint16 x, Uint16 y, Uint8 c );

/* get the palette index of a pixel on screen */
Uint8 vga_13hgetpixel( Uint16 x, Uint16 y );

/* set the 256-color palette */
void vga_13hsetpal( Uint8 *pal );

/* get the 256-color palette */
void vga_13hgetpal( Uint8 *pal );

/* draw a rectangle filled with one color */
void vga_13hdrawrect( Uint16 x, Uint16 y, Uint16 w, Uint16 h, Uint8 c );

/* draw an image on the entire screen */
void vga_13hfullblit( Uint8 *img, Uint16 palshift );
#endif
