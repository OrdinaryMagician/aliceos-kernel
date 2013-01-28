/*
	vgaimgtxt.c : Block graphics mode (80x50).
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <vgaimgtxt.h>

Uint16 *vmem = (Uint16*)0xB8000;

/* draw image at a specific position */
void vga_drawimg( bitmap_t *img, Uint16 posx, Uint16 posy )
{
	Uint16 rows = 0, lines = 0;
	Uint32 i = 0;
	do
	{
		vga_putbloxel(posx+rows,posy+lines,(*img).data[i]);
		rows++;
		if ( rows >= (*img).w )
		{
			rows = 0;
			lines++;
		}
		i++;
	}
	while ( (rows < (*img).w) && (lines < (*img).h) );
}

/* fill screen with a 80x50 bitmap */
void vga_fullblit( Uint8 *img )
{
	Uint16 rows = 0, lines = 0;
	Uint32 i = 0;
	while ( i < 4000 )
	{
		vga_putbloxel(rows,lines,img[i]);
		rows++;
		if ( rows >= 80 )
		{
			rows = 0;
			lines++;
		}
		i++;
	}
}

/* draw a solid rectangle */
void vga_drawrect( Uint16 x, Uint16 y, Uint16 w, Uint16 h, Uint8 c )
{
	Uint16 px, py;
	Uint16 lx, ly;
	px = x;
	py = y;
	lx = x+w;
	ly = y+h;
	do
	{
		vga_putbloxel(px,py,c);
		px++;
		if ( px >= lx )
		{
			px = x;
			py++;
		}
	}
	while ( (px < lx) && (py < ly) );
}

/* get the color of a specific bloxel */
Uint8 vga_getbloxel( Uint16 posx, Uint16 posy )
{
	Uint16 *pair = vmem + posx + (posy/2)*80;
	if ( posy%2 )
		return ((*pair)&0xF000)>>12;
	else
		return ((*pair)&0x0F00)>>8;
}

/* set the color of a specific bloxel */
void vga_putbloxel( Uint16 posx, Uint16 posy, Uint8 color )
{
	if ( color > 0x0F )
		return;
	Uint16 *pair = vmem + posx + (posy/2)*80;
	if ( posy%2 )
	{
		*pair &= 0x0FFF;
		*pair |= (Uint16)color<<12;
	}
	else
	{
		*pair &= 0xF0FF;
		*pair |= (Uint16)color<<8;
	}
}

/* initiate block graphics mode */
void vga_initimgtxt( void )
{
	/* reposition and hide cursor */
	vga_curset(0,0);
	vga_hidecursor();
	/* some necessary attribute changes */
	vga_set8dot();
	vga_disableblink();
	/* fill screen with upper halfblocks and set all colors to black */
	memsetw(vmem,0x00DF,80*25);
}

/* return to normal text mode */
void vga_exitimgtxt( void )
{
	/* show cursor again */
	vga_showcursor();
	/* blank screen */
	vga_clr_s();
	/* reset attributes */
	vga_set9dot();
	vga_enableblink();
}
