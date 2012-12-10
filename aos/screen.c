/*
	screen.c : Simple VGA text drawing funtions.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include "port.h"
#include "string.h"
#include "screen.h"

unsigned short *textmemptr;
int attrib = 0x0F;
int csr_x = 0, csr_y = 0;

void scroll( void )
{
	unsigned blank, temp;
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
	unsigned temp;
	temp = csr_y*80+csr_x;
	outb(0x3D4,14);
	outb(0x3D5,temp>>8);
	outb(0x3D4,15);
	outb(0x3D5,temp);
}

extern void cls( void )
{
	unsigned blank;
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
	unsigned short *where;
	unsigned attr = attrib << 8;
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

extern void text_setcolor( unsigned char fg, unsigned char bg )
{
	attrib = (bg << 4) | (fg & 0x0F);
}

extern void init_video( void )
{
	textmemptr = (unsigned short *)0xB8000;
	cls();
}
