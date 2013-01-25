/*
	vgafont.c : BIOS font patches.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <vgafont.h>

/* apply patches to the BIOS font, save a local copy */
void vga_initfont( void )
{
	Uint8 chr[16];

	/* correct misproportioned half block characters 0xDF and 0xDC */
	vga_getglyph(0xDF,&chr[0]);
	chr[7] = 0xFF;
	vga_setglyph(0xDF,&chr[0]);

	vga_getglyph(0xDC,&chr[0]);
	chr[7] = 0x00;
	vga_setglyph(0xDC,&chr[0]);
	
	/* save font */
	vga_getfont(&biosfnt[0]);
}
