/*
	vgaplanar16.c : functions for drawing in 640x480 planar 16 color mode.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <vgaplanar16.h>

Uint8 *p16vmem = (Uint8*)0xA0000;

void vga_p16init( void )
{
	Uint8 p;
	for ( p=0; p<4; p++ )
	{
		vga_planeswitch(p);
		memset(p16vmem,0x00,65536);
	}
}

void vga_p16putpixelsp( Uint16 x, Uint16 y, Uint8 on )
{
	on &= 0x01;
	Uint8 p;
	Uint16 coord = (x+y*640)/8;
	Uint8 off = 7-(x%8);
	p16vmem[coord] &= ~(1<<off);
	p16vmem[coord] |= on<<off;
}

Uint8 vga_p16getpixelsp( Uint16 x, Uint16 y )
{
	Uint8 p;
	Uint16 coord = (x+y*640)/8;
	Uint8 off = 7-(x%8);
	return (p16vmem[coord]>>off)&1;
}

void vga_p16putpixel( Uint16 x, Uint16 y, Uint8 c )
{
	c &= 0x0F;
	Uint8 p;
	Uint16 coord = (x+y*640)/8;
	Uint8 off = 7-(x%8);
	Uint8 c2[4] = { c&1, (c&2)>>1, (c&4)>>2, (c&8)>>3 };
	for ( p=0; p<4; p++ )
	{
		vga_planeswitch(p);
		p16vmem[coord] &= ~(1<<off);
		p16vmem[coord] |= c2[p]<<off;
	}
}

Uint8 vga_p16getpixel( Uint16 x, Uint16 y )
{
	Uint8 p;
	Uint16 coord = (x+y*640)/8;
	Uint8 off = 7-(x%8);
	Uint8 c2[4];
	for ( p=0; p<4; p++ )
	{
		vga_planeswitch(p);
		c2[p] = (p16vmem[coord]>>off)&1;
	}
	return c2[0]&(c2[1]<<1)&(c2[2]<<2)&(c2[3]<<4);
}
