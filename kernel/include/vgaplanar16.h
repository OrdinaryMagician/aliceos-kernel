/*
	vgaplanar16.h : functions for drawing in planar 16 color mode.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef VGAPLANAR16_H
#define VGAPLANAR16_H
#include <types.h>
#include <vgareg.h>
#include <vgafont.h>
#include <vgamisc.h>

void vga_p16init( void );

void vga_p16putpixel( Uint16 x, Uint16 y, Uint8 c );
void vga_p16putpixelsp( Uint16 x, Uint16 y, Uint8 on );
#endif
