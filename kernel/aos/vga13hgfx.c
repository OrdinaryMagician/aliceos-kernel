/*
	vga13hgfx.c : mode 13h graphics.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <vga13hgfx.h>

Uint8 *vidmem = (Uint8*)0xA0000; /* 0xA0000-0xBFFFF (128KiB) */

/* draw a pixel on screen */
void vga_13hputpixel( Uint16 x, Uint16 y, Uint8 c )
{
	x %= 320;
	y %= 200;
	vidmem[x+y*320] = c;
}

/* get the palette index of a pixel on screen */
Uint8 vga_13hgetpixel( Uint16 x, Uint16 y )
{
	x %= 320;
	y %= 200;
	return vidmem[x+y*320];
}

/* set the 256-color palette */
void vga_13hsetpal( Uint8 *pal )
{
	int i;
	for ( i=0; i<256; i++ )
	{
		outport_b(VGA_DAC_ADWM,i);
		outport_b(VGA_DAC_DATA,(*pal++));
		outport_b(VGA_DAC_DATA,(*pal++));
		outport_b(VGA_DAC_DATA,(*pal++));
	}
}

/* get the 256-color palette */
void vga_13hgetpal( Uint8 *pal )
{
	int i;
	for ( i=0; i<256; i++ )
	{
		outport_b(VGA_DAC_ADRM,i);
		*(pal++) = inport_b(VGA_DAC_DATA);
		*(pal++) = inport_b(VGA_DAC_DATA);
		*(pal++) = inport_b(VGA_DAC_DATA);
	}
}

/* draw a rectangle filled with one color */
void vga_13hdrawrect( Uint16 x, Uint16 y, Uint16 w, Uint16 h, Uint8 c )
{
	Uint16 px, py;
	Uint16 lx, ly;
	px = x;
	py = y;
	lx = x+w;
	ly = y+h;
	do
	{
		vga_13hputpixel(px,py,c);
		px++;
		if ( px >= lx )
		{
			px = x;
			py++;
		}
	}
	while ( (px < lx) && (py < ly) );
}

/* draw an image on the entire screen */
void vga_13hfullblit( Uint8 *img, Uint16 palshift )
{
	Uint16 rows = 0, lines = 0;
	Uint32 i = 0;
	while ( i < 64000 )
	{
		vga_13hputpixel(rows,lines,(img[i]+palshift)%256);
		rows++;
		if ( rows >= 320 )
		{
			rows = 0;
			lines++;
		}
		i++;
	}
}
