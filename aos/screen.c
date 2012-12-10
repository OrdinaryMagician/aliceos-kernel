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
	if ( csr_y >= 25 )
	{
		int i;
		memcpyw(textmemptr,textmemptr+80,24*80);
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

extern void clear_screen( void )
{
	Uint16 blank;
	blank = 0x20 | (attrib << 8);
	memsetw(textmemptr,blank,80*25);
}

extern void clear_line( void )
{
	Uint16 blank;
	blank = 0x20 | (attrib << 8);
	memsetw(textmemptr+csr_y*80,blank,80);
}

extern void clear_line_r( void )
{
	Uint16 blank;
	blank = 0x20 | (attrib << 8);
	memsetw(textmemptr+csr_x+csr_y*80,blank,80-csr_x);
}

extern void clear_line_l( void )
{
	Uint16 blank;
	blank = 0x20 | (attrib << 8);
	memsetw(textmemptr+csr_y*80,blank,csr_x);
}

extern void clear_screen_r( void )
{
	Uint16 blank;
	int i;
	blank = 0x20 | (attrib << 8);
	memsetw(textmemptr+csr_x+csr_y*80,blank,80*25-(csr_x+csr_y*80));
}

extern void clear_screen_l( void )
{
	Uint16 blank;
	int i;
	blank = 0x20 | (attrib << 8);
	memsetw(textmemptr,blank,csr_x+csr_y*80);
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

extern void putld( Sint64 d )
{
	if ( d == 0 )
	{
		putc('0');
		return;
	}
	Sint64 dig = (d<0) ? -d : d;
	char c[19];
	int i = 0;
	while ( dig > 0 )
	{
		c[i] = '0'+dig%10;
		dig /= 10;
		i++;
	}
	c[i] = 0;
	char cr[19];
	cr[i--] = 0;
	int j = 0;
	while ( i >= 0 )
		cr[i--] = c[j++];
	if ( d<0 )
		putc('-');
	puts(cr);
}

extern void putd( Sint32 d )
{
	if ( d == 0 )
	{
		putc('0');
		return;
	}
	Sint32 dig = (d<0) ? -d : d;
	char c[10];
	int i = 0;
	while ( dig > 0 )
	{
		c[i] = '0'+dig%10;
		dig /= 10;
		i++;
	}
	c[i] = 0;
	char cr[10];
	cr[i--] = 0;
	int j = 0;
	while ( i >= 0 )
		cr[i--] = c[j++];
	if ( d<0 )
		putc('-');
	puts(cr);
}

extern void putlu( Uint64 u )
{
	if ( u == 0 )
	{
		putc('0');
		return;
	}
	Uint64 dig = u;
	char c[20];
	int i = 0;
	while ( dig > 0 )
	{
		c[i] = '0'+dig%10;
		dig /= 10;
		i++;
	}
	c[i] = 0;
	char cr[20];
	cr[i--] = 0;
	int j = 0;
	while ( i >= 0 )
		cr[i--] = c[j++];
	puts(cr);
}

extern void putu( Uint32 u )
{
	if ( u == 0 )
	{
		putc('0');
		return;
	}
	Uint32 dig = u;
	char c[10];
	int i = 0;
	while ( dig > 0 )
	{
		c[i] = '0'+dig%10;
		dig /= 10;
		i++;
	}
	c[i] = 0;
	char cr[10];
	cr[i--] = 0;
	int j = 0;
	while ( i >= 0 )
		cr[i--] = c[j++];
	puts(cr);
}

extern void putlh( Uint64 h )
{
	Uint64 tmp;
	puts("0x");
	Uint8 nozero = 1;
	int i;
	for ( i=60; i>0; i-=4 )
	{
		tmp = (h>>i)&0xF;
		if ( (tmp == 0) && (nozero != 0) )
			continue;
		nozero = 0;
		putc((tmp >= 0xA) ? (tmp-(0xA+'a')) : (tmp+'0'));
	}
	tmp = h&0xF;
	putc((tmp >= 0xA) ? (tmp-(0xA+'a')) : (tmp+'0'));
}

extern void puth( Uint32 h )
{
	Uint32 tmp;
	puts("0x");
	Uint8 nozero = 1;
	int i;
	for ( i=28; i>0; i-=4 )
	{
		tmp = (h>>i)&0xF;
		if ( (tmp == 0) && (nozero != 0) )
			continue;
		nozero = 0;
		putc((tmp >= 0xA) ? (tmp-(0xA+'a')) : (tmp+'0'));
	}
	tmp = h&0xF;
	putc((tmp >= 0xA) ? (tmp-(0xA+'a')) : (tmp+'0'));
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
	move_csr();
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
	move_csr();
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
	clear_screen();
	cur_set(0,0);
}
