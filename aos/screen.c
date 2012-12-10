/*
	screen.c : Simple VGA text drawing functions.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include "port.h"
#include "string.h"
#include "screen.h"

Uint16 *textmemptr;
int attrib = 0x0F;
int csr_x = 0, csr_y = 0;

void scroll( void )
{
	Uint16 blank;
	blank = 0x20 | (attrib << 8);
	if ( csr_y >=25 )
	{
		memcpy(textmemptr,textmemptr+80,24*80);
		memsetw(textmemptr+24*80,blank,80);
		csr_y = 24;
	}
}

void move_csr( void )
{
	Uint16 temp;
	temp = csr_y*80+csr_x;
	outb(0x3D4,14);
	outb(0x3D5,temp>>8);
	outb(0x3D4,15);
	outb(0x3D5,temp);
}

extern void cls( void )
{
	Uint16 blank;
	int i;
	blank = 0x20 | (attrib << 8);
	for ( i=0; i<25; i++ )
		memsetw(textmemptr+i*80,blank,80);
	csr_x = 0;
	csr_y = 0;
	move_csr();
}

extern void putc( char c)
{
	Uint16 *where;
	Uint16 attr = attrib << 8;
	if ( c == '\b' )
		csr_x -= (csr_x != 0) ? 1 : 0;
	else if ( c == '\t' )
		csr_x = (csr_x+8) & ~(8-1);
	else if ( c == '\r' )
		csr_x = 0;
	else if ( c == '\n' )
	{
		csr_x = 0;
		csr_y++;
	}
	else if ( c >= ' ' )
	{
		where = textmemptr + (csr_y * 80 + csr_x);
		*where = c | attr;
		csr_x++;
	}
	if ( csr_x >= 80 )
	{
		csr_x = 0;
		csr_y++;
	}
	scroll();
	move_csr();
}

extern void puts( char *text )
{
	int i;
	for ( i=0; i<strlen(text); i++ )
		putc(text[i]);
}

extern void cur_set( int x, int y )
{
	while ( x >= 80 )
		x -= 80;
	while ( x < 0 )
		x += 80;
	while ( y >= 25 )
		y -= 25;
	while ( y < 0 )
		y += 25;
	csr_x = x;
	csr_y = y;
}

extern void cur_move( int ox, int oy )
{
	int px, py;
	px = csr_x+ox;
	py = csr_y+oy;
	while ( px >= 80 )
		px -= 80;
	while ( px < 0 )
		px += 80;
	while ( py >= 25 )
		py -= 25;
	while ( py < 0 )
		py += 25;
	csr_x = px;
	csr_y = py;
}

extern void cur_get( int *x, int *y )
{
	*x = csr_x;
	*y = csr_y;
}

extern void text_getcolor( Uint8 *fg, Uint8 *bg )
{
	*bg = (attrib>>4)&0x0F;
	*fg = attrib&0x0F;
}

extern void text_setcolor( Uint8 fg, Uint8 bg )
{
	attrib = (bg << 4) | (fg & 0x0F);
}

extern void init_video( void )
{
	textmemptr = (Uint16*)0xB8000;
	cls();
}
