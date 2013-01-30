/*
	vgaplanar16.c : functions for drawing in 640x480 planar 16 color mode.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <vgaplanar16.h>

Uint8 *p16vmem = (Uint8*)0xA0000; /* 0xA0000-0xAFFFF (64KiB) */

/* Initialize 640x480 planar 16 color mode */
void vga_p16init( void )
{
	Uint8 p;
	for ( p=0; p<4; p++ )
	{
		vga_planeswitch(p);
		memset(p16vmem,0x00,65536);
	}
}

/* draw an image on the whole screen using the per-channel blit method */
void vga_p16fullblit_channel( Uint8 *img )
{
	Uint8 p;
	Uint8 c;
	Uint16 px, py;
	Uint16 lx, ly;
	for ( p=0; p<4; p++ )
	{
		vga_planeswitch(p);
		px = 0;
		py = 0;
		lx = 640;
		ly = 480;
		do
		{
			c = img[px+py*640];
			vga_p16putpixelsp(px,py,(c>>p)&1);
			px++;
			py = (px>=lx)?(py+1):py;
			px = (px>=lx)?0:px;
		}
		while ( (px < lx) && (py < ly) );
	}
}

/* draw an image on the whole screen using the per-pixel blit method */
void vga_p16fullblit_pixel( Uint8 *img )
{
	Uint8 c;
	Uint16 px, py;
	Uint16 lx, ly;
	px = 0;
	py = 0;
	lx = 640;
	ly = 480;
	do
	{
		c = img[px+py*640];
		vga_planeswitch(0);
		vga_p16putpixelsp(px,py,(c)&1);
		vga_planeswitch(1);
		vga_p16putpixelsp(px,py,(c>>1)&1);
		vga_planeswitch(2);
		vga_p16putpixelsp(px,py,(c>>2)&1);
		vga_planeswitch(3);
		vga_p16putpixelsp(px,py,(c>>3)&1);
		px++;
		py = (px>=lx)?(py+1):py;
		px = (px>=lx)?0:px;
	}
	while ( (px < lx) && (py < ly) );
}

/* draw an image on the whole screen */
void vga_p16fullblit( Uint8 *img, Uint8 bmode )
{
	switch ( bmode )
	{
	case BLIT_CHANNEL:
		vga_p16fullblit_channel(img);
		break;
	case BLIT_PIXEL:
	default:
		vga_p16fullblit_pixel(img);
		break;
	}
}

/* Writes pixels one by one, iterating between planes for each channel filled */
void vga_p16drawrect_channel( Uint16 x, Uint16 y, Uint16 w, Uint16 h, Uint8 c )
{
	Uint8 p;
	Uint16 px, py;
	Uint16 lx, ly;
	for ( p=0; p<4; p++ )
	{
		vga_planeswitch(p);
		px = x;
		py = y;
		lx = x+w;
		ly = y+h;
		do
		{
			vga_p16putpixelsp(px,py,(c>>p)&1);
			px++;
			py = (px>=lx)?(py+1):py;
			px = (px>=lx)?x:px;
		}
		while ( (px < lx) && (py < ly) );
	}
}

/* Writes pixels one by one, iterating between planes for each bit of each pixel */
void vga_p16drawrect_pixel( Uint16 x, Uint16 y, Uint16 w, Uint16 h, Uint8 c )
{
	Uint16 px, py;
	Uint16 lx, ly;
	px = x;
	py = y;
	lx = x+w;
	ly = y+h;
	do
	{
		vga_planeswitch(0);
		vga_p16putpixelsp(px,py,(c)&1);
		vga_planeswitch(1);
		vga_p16putpixelsp(px,py,(c>>1)&1);
		vga_planeswitch(2);
		vga_p16putpixelsp(px,py,(c>>2)&1);
		vga_planeswitch(3);
		vga_p16putpixelsp(px,py,(c>>3)&1);
		px++;
		py = (px>=lx)?(py+1):py;
		px = (px>=lx)?x:px;
	}
	while ( (px < lx) && (py < ly) );
}

/* Draw a filled rectangle */
void vga_p16drawrect( Uint16 x, Uint16 y, Uint16 w, Uint16 h, Uint8 c, Uint8 bmode )
{
	switch ( bmode )
	{
	case BLIT_CHANNEL:
		vga_p16drawrect_channel(x,y,w,h,c);
		break;
	case BLIT_PIXEL:
	default:
		vga_p16drawrect_pixel(x,y,w,h,c);
		break;
	}
}

/* change one of the four bits of a pixel */
void vga_p16putpixelsp( Uint16 x, Uint16 y, Uint8 on )
{
	on &= 0x01;
	Uint16 coord = (x+y*640)/8;
	Uint8 off = 7-(x%8);
	p16vmem[coord] &= ~(1<<off);
	p16vmem[coord] |= on<<off;
}

/* get one of the four bits of a pixel */
Uint8 vga_p16getpixelsp( Uint16 x, Uint16 y )
{
	Uint16 coord = (x+y*640)/8;
	Uint8 off = 7-(x%8);
	return (p16vmem[coord]>>off)&1;
}

/* change a pixel */
void vga_p16putpixel( Uint16 x, Uint16 y, Uint8 c )
{
	c &= 0x0F;
	Uint8 p;
	Uint16 coord = (x+y*640)/8;
	Uint8 off = 7-(x%8);
	Uint8 c2[4] = { c&1, (c>>1)&1, (c>>2)&1, (c>>3)&1 };
	for ( p=0; p<4; p++ )
	{
		vga_planeswitch(p);
		p16vmem[coord] &= ~(1<<off);
		p16vmem[coord] |= c2[p]<<off;
	}
}

/* get a pixel */
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
