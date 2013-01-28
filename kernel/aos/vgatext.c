/*
	vgatext.c : Standard VGA text mode functionality.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <vgatext.h>

Uint16 *vgamem = (Uint16*)0xB8000;	/* pointer to video memory */
Uint8 attrib = 0x07;	/* current text attributes */
Sint16 csr_x = 0, csr_y = 0; /* cursor position */
Uint8 doscroll = 1;	/* should perform scrolling? */
Uint8 cropbeg = 0, cropend = 25;

/* toggle automatic scrolling */
void vga_tscroll( Uint8 on )
{
	doscroll = on;
}

/* change the area where scrolling can take effect */
void vga_setcroparea( Uint8 start, Uint8 end )
{
	if ( start > 24 )
		start = 24;
	if ( end <= start )
		end = start+1;
	cropbeg = start;
	cropend = end;
}

/* scroll if needed */
void vga_scroll( void )
{
	if ( csr_y >= cropend )
	{
		csr_y = cropend-1;
		if ( !doscroll )
			return;
		memcpyw(vgamem+cropbeg*80,vgamem+(cropbeg+1)*80,((cropend-1)-cropbeg)*80);
		memsetw(vgamem+(cropend-1)*80,0x0720,80);
	}
}

/* update cursor position */
void vga_updatecsr( void )
{
	Uint16 tmp = csr_y*80+csr_x;
	vga_setreg(VGA_CRTC,VGA_CRTC_CSRLCH,tmp>>8);
	vga_setreg(VGA_CRTC,VGA_CRTC_CSRLCL,tmp);
}

/* clear entire screen */
void vga_clr_s( void )
{
	memsetw(vgamem,0x0720,80*25);
}

/* clear screen to the left of the cursor */
void vga_clr_sl( void )
{
	memsetw(vgamem,0x0720,csr_x+csr_y*80);
}

/* clear screen to the right of the cursor */
void vga_clr_sr( void )
{
	memsetw(vgamem+csr_x+csr_y*80,0x0720,80*25-(csr_x+csr_y*80));
}

/* clear current line */
void vga_clr_l( void )
{
	memsetw(vgamem+csr_y*80,0x0720,80);
}

/* clear everything inside the current line to the left of the cursor */
void vga_clr_ll( void )
{
	memsetw(vgamem+csr_y*80,0x0720,csr_x);
}

/* clear everything inside the current line to the right of the cursor */
void vga_clr_lr( void )
{
	memsetw(vgamem+csr_x+csr_y*80,0x0720,80-csr_x);
}

/* set cursor position directly */
void vga_curset( Sint32 x, Sint32 y )
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
	vga_updatecsr();
}

/* move cursor by offset */
void vga_curmv( Sint32 ox, Sint32 oy )
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
	vga_updatecsr();
}

/* get current cursor position */
void vga_curget( Sint32 *x, Sint32 *y )
{
	*x = csr_x;
	*y = csr_y;
}

/* get text attributes */
void vga_getattr( Uint8 *fg, Uint8 *bg )
{
	*bg = (attrib>>4)&0x0F;
	*fg = attrib&0x0F;
}

/* set text attributes */
void vga_setattr( Uint8 fg, Uint8 bg )
{
	attrib = ((bg&0x0F)<<4)|(fg&0x0F);
}

/* set character at position, no scrolling or cursor movement */
void vga_setc( Uint16 pos, Uint8 c, Uint8 attr )
{
	*(vgamem+pos) = c|(attr<<8);
}

/* put a single character on screen */
void vga_putc( char c )
{
	if ( c == '\b' )
		csr_x -= (csr_x!=0)?1:0;
	else if ( c == '\t' )
		csr_x = (csr_x+8) & ~(8-1);
	else if ( c == '\r' )
		csr_x = 0;
	else if ( c == '\n' )
	{
		csr_x = 0;
		csr_y++;
	}
	else
	{
		vga_setc(csr_x+csr_y*80,c,attrib);
		csr_x++;
	}
	if ( csr_x >= 80 )
	{
		csr_x = 0;
		csr_y++;
	}
	vga_scroll();
	vga_updatecsr();
}

/* put an entire string on screen */
void vga_puts( char *s )
{
	while ( *s )
		vga_putc(*(s++));
}

/* put a character many times on screen */
void vga_putmc( char c, Sint32 i )
{
	while ( i-- > 0 )
		vga_putc(c);
}

/* put a base 10 unsigned integer on screen */
void vga_putu( Uint64 val, Uint16 width, Uint8 zeropad )
{
	if ( !width )
	{
		char c[20];
		Sint32 i = 0;
		do
		{
			c[i++] = '0'+val%10;
			val /= 10;
		}
		while ( val != 0 );
		while ( i > 0 )
			vga_putc(c[--i]);
	}
	else
	{
		char c[width];
		Sint32 i = 0;
		do
		{
			c[i++] = '0'+val%10;
			val /= 10;
		}
		while ( (val != 0) && (i < width) );
		while ( i < width )
			c[i++] = (zeropad)?'0':' ';
		while ( i > 0 )
			vga_putc(c[--i]);
	}
}

/* put a base 10 signed integer on screen */
void vga_putd( Sint64 val, Uint16 width, Uint8 zeropad )
{
	Uint8 isneg = (val<0);
	val = abs(val);
	if ( !width )
	{
		char c[19];
		Sint32 i = 0;
		do
		{
			c[i++] = '0'+val%10;
			val /= 10;
		}
		while ( val != 0 );
		if ( isneg )
			vga_putc('-');
		while ( i > 0 )
			vga_putc(c[--i]);
	}
	else
	{
		char c[width];
		Sint32 i = 0;
		do
		{
			c[i++] = '0'+val%10;
			val /= 10;
		}
		while ( (val != 0) && (i < width) );
		while ( i < width )
			c[i++] = (zeropad)?'0':' ';
		if ( isneg )
			vga_putc('-');
		while ( i > 0 )
			vga_putc(c[--i]);
	}
}

/* put a base 16 unsigned integer on screen */
void vga_puth( Uint64 val, Uint16 width, Uint8 zeropad )
{
	if ( !width )
	{
		char c[8];
		Sint32 i = 0;
		do
		{
			c[i++] = ((val&0x0F)>0x09)?('A'+(val&0x0F)-0x0A):('0'+(val&0x0F));
			val >>= 4;
		}
		while ( val != 0 );
		while ( i > 0 )
			vga_putc(c[--i]);
	}
	else
	{
		char c[width];
		Sint32 i = 0;
		do
		{
			c[i++] = ((val&0x0F)>0x09)?('A'+(val&0x0F)-0x0A):('0'+(val&0x0F));
			val >>= 4;
		}
		while ( (val != 0) && (i < width) );
		while ( i < width )
			c[i++] = (zeropad)?'0':' ';
		while ( i > 0 )
			vga_putc(c[--i]);
	}
}

/* put a base 8 unsigned integer on screen */
void vga_puto( Uint64 val, Uint16 width, Uint8 zeropad )	/* no jokes about the function name, please */
{
	if ( !width )
	{
		char c[8];
		Sint32 i = 0;
		do
		{
			c[i++] ='0'+(val&0x07);
			val >>= 3;
		}
		while ( val != 0 );
		while ( i > 0 )
			vga_putc(c[--i]);
	}
	else
	{
		char c[width];
		Sint32 i = 0;
		do
		{
			c[i++] ='0'+(val&0x07);
			val >>= 3;
		}
		while ( (val != 0) && (i < width) );
		while ( i < width )
			c[i++] = (zeropad)?'0':' ';
		while ( i > 0 )
			vga_putc(c[--i]);
	}
}

/* set the 16-color 6-bit palettes */
void vga_setpal( Uint8 *pal )
{
	int i;
	/* CGA->EGA mapping */
	int map[16] = {0,1,2,3,4,5,20,7,56,57,58,59,60,61,62,63};
	for ( i=0; i<16; i++ )
	{
		outport_b(VGA_DAC_ADWM,map[i]);
		outport_b(VGA_DAC_DATA,pal[i*3]);
		outport_b(VGA_DAC_DATA,pal[i*3+1]);
		outport_b(VGA_DAC_DATA,pal[i*3+2]);
	}
}

/* get the 16-color 6-bit palettes */
void vga_getpal( Uint8 *pal )
{
	int i;
	/* CGA->EGA mapping */
	int map[16] = {0,1,2,3,4,5,20,7,56,57,58,59,60,61,62,63};
	for ( i=0; i<16; i++ )
	{
		outport_b(VGA_DAC_ADRM,map[i]);
		pal[i*3] = inport_b(VGA_DAC_DATA);
		pal[i*3+1] = inport_b(VGA_DAC_DATA);
		pal[i*3+2] = inport_b(VGA_DAC_DATA);
	}
}

/* change the font */
void vga_setfont( Uint8 *font )
{
	/* need to change some stuff, like plane to write to and stuff */
	vga_setreg(VGA_SEQ,VGA_SEQ_MMASK,0x04);
	vga_setreg(VGA_SEQ,VGA_SEQ_CMSEL,0x00);
	Uint8 old_smmod = vga_getreg(VGA_SEQ,VGA_SEQ_SMMOD);
	vga_setreg(VGA_SEQ,VGA_SEQ_SMMOD,0x06);
	vga_setreg(VGA_GC,VGA_GC_RDMSEL,0x02);
	Uint8 old_gfxmod = vga_getreg(VGA_GC,VGA_GC_GFXMOD);
	vga_setreg(VGA_GC,VGA_GC_GFXMOD,0x00);
	int i, j;
	/* copy character map, skip 16 bytes of padding for each glyph */
	Uint8 *fntmem = (Uint8*)0xB8000;
	for ( i=0; i<256; i++ )
	{
		for ( j=0; j<16; j++ )
			*(fntmem++) = *(font++);
		fntmem += 16;
	}
	/* restore to defaults */
	vga_setreg(VGA_SEQ,VGA_SEQ_MMASK,0x03);
	vga_setreg(VGA_SEQ,VGA_SEQ_SMMOD,old_smmod);
	vga_setreg(VGA_GC,VGA_GC_RDMSEL,0x00);
	vga_setreg(VGA_GC,VGA_GC_GFXMOD,old_gfxmod);
}

/* get the current font */
void vga_getfont( Uint8 *font )
{
	/* need to change some stuff, like plane to write to and stuff */
	vga_setreg(VGA_SEQ,VGA_SEQ_MMASK,0x04);
	vga_setreg(VGA_SEQ,VGA_SEQ_CMSEL,0x00);
	Uint8 old_smmod = vga_getreg(VGA_SEQ,VGA_SEQ_SMMOD);
	vga_setreg(VGA_SEQ,VGA_SEQ_SMMOD,0x06);
	vga_setreg(VGA_GC,VGA_GC_RDMSEL,0x02);
	Uint8 old_gfxmod = vga_getreg(VGA_GC,VGA_GC_GFXMOD);
	vga_setreg(VGA_GC,VGA_GC_GFXMOD,0x00);
	int i, j;
	/* copy character map, skip 16 bytes of padding for each glyph */
	Uint8 *fntmem = (Uint8*)0xB8000;
	for ( i=0; i<256; i++ )
	{
		for ( j=0; j<16; j++ )
			*(font++) = *(fntmem++);
		fntmem += 16;
	}
	/* restore to defaults */
	vga_setreg(VGA_SEQ,VGA_SEQ_MMASK,0x03);
	vga_setreg(VGA_SEQ,VGA_SEQ_SMMOD,old_smmod);
	vga_setreg(VGA_GC,VGA_GC_RDMSEL,0x00);
	vga_setreg(VGA_GC,VGA_GC_GFXMOD,old_gfxmod);
}

/* change a specific glyph in the character map */
void vga_setglyph( Uint8 val, Uint8 *data )
{
	/* need to change some stuff, like plane to write to and stuff */
	vga_setreg(VGA_SEQ,VGA_SEQ_MMASK,0x04);
	vga_setreg(VGA_SEQ,VGA_SEQ_CMSEL,0x00);
	Uint8 old_smmod = vga_getreg(VGA_SEQ,VGA_SEQ_SMMOD);
	vga_setreg(VGA_SEQ,VGA_SEQ_SMMOD,0x06);
	vga_setreg(VGA_GC,VGA_GC_RDMSEL,0x02);
	Uint8 old_gfxmod = vga_getreg(VGA_GC,VGA_GC_GFXMOD);
	vga_setreg(VGA_GC,VGA_GC_GFXMOD,0x00);
	int i;
	Uint8 *fntmem = (Uint8*)0xB8000;
	/* skip characters */
	for ( i=0; i<val; i++ )
		fntmem += 32;
	for ( i=0; i<16; i++ )
		*(fntmem++) = *(data++);
	/* restore to defaults */
	vga_setreg(VGA_SEQ,VGA_SEQ_MMASK,0x03);
	vga_setreg(VGA_SEQ,VGA_SEQ_SMMOD,old_smmod);
	vga_setreg(VGA_GC,VGA_GC_RDMSEL,0x00);
	vga_setreg(VGA_GC,VGA_GC_GFXMOD,old_gfxmod);
}

/* retrieve a specific glyph in the character map */
void vga_getglyph( Uint8 val, Uint8 *data )
{
	/* need to change some stuff, like plane to write to and stuff */
	vga_setreg(VGA_SEQ,VGA_SEQ_MMASK,0x04);
	vga_setreg(VGA_SEQ,VGA_SEQ_CMSEL,0x00);
	Uint8 old_smmod = vga_getreg(VGA_SEQ,VGA_SEQ_SMMOD);
	vga_setreg(VGA_SEQ,VGA_SEQ_SMMOD,0x06);
	vga_setreg(VGA_GC,VGA_GC_RDMSEL,0x02);
	Uint8 old_gfxmod = vga_getreg(VGA_GC,VGA_GC_GFXMOD);
	vga_setreg(VGA_GC,VGA_GC_GFXMOD,0x00);
	int i;
	Uint8 *fntmem = (Uint8*)0xB8000;
	/* skip characters */
	for ( i=0; i<val; i++ )
		fntmem += 32;
	for ( i=0; i<16; i++ )
		*(data++) = *(fntmem++);
	/* restore to defaults */
	vga_setreg(VGA_SEQ,VGA_SEQ_MMASK,0x03);
	vga_setreg(VGA_SEQ,VGA_SEQ_SMMOD,old_smmod);
	vga_setreg(VGA_GC,VGA_GC_RDMSEL,0x00);
	vga_setreg(VGA_GC,VGA_GC_GFXMOD,old_gfxmod);
}

/* hide cursor */
void vga_hidecursor( void )
{
	Uint8 reg = vga_getreg(VGA_CRTC,VGA_CRTC_CSRSTR);
	reg |= (1<<5);
	vga_setreg(VGA_CRTC,VGA_CRTC_CSRSTR,reg);
}

/* show cursor */
void vga_showcursor( void )
{
	Uint8 reg = vga_getreg(VGA_CRTC,VGA_CRTC_CSRSTR);
	reg &= ~(1<<5);
	vga_setreg(VGA_CRTC,VGA_CRTC_CSRSTR,reg);
}
