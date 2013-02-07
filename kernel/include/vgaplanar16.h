/*
	vgaplanar16.h : functions for drawing in planar 16 color mode.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
	
	[DEPRECATED]
*/
#ifndef VGAPLANAR16_H
#define VGAPLANAR16_H
#include <types.h>
#include <vgareg.h>
#include <vgafont.h>
#include <vgamisc.h>

#define BLIT_PIXEL   0
#define BLIT_CHANNEL 1

/* Initialize 640x480 planar 16 color mode */
void vga_p16init( void );

/* draw an image on the whole screen */
void vga_p16fullblit( Uint8 *img, Uint8 bmode );

/* Draw a filled rectangle */
void vga_p16drawrect( Uint16 x, Uint16 y, Uint16 w, Uint16 h, Uint8 c, Uint8 bmode );

/* change one of the four bits of a pixel */
void vga_p16putpixel( Uint16 x, Uint16 y, Uint8 c );

/* get one of the four bits of a pixel */
Uint8 vga_p16getpixel( Uint16 x, Uint16 y );

/* change a pixel */
void vga_p16putpixelsp( Uint16 x, Uint16 y, Uint8 on );

/* get a pixel */
Uint8 vga_p16getpixelsp( Uint16 x, Uint16 y );
#endif
