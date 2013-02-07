/*
	vgamode13h.c : Mode 13h (linear 320x200 256 colors).
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <vgamode13h.h>
#include <vgareg.h>
#include <vgamisc.h>
#include <memops.h>
#include <vgaloadimg.h>
#include <port.h>
#include <helpers.h>

/* mode 13h variables */
Uint8 *m13h_mem = (Uint8*)0xA0000; /* memory area */
fnt_t m13h_fnt; /* font (currently empty) */
Uint16 m13h_cx = 0, m13h_cy = 0; /* cursor position for text */
Uint8 m13h_cv = 1; /* cursor visibility for text */
Uint16 m13h_fbw = 40, m13h_fbh = 25; /* framebuffer console columns and rows */
Uint8 m13h_attrs[3] = {7,0,0}; /* current text attributes */

/* mode 13h function prototypes */
void m13h_setmode( void );
void m13h_setpal( Uint8* pal );
void m13h_getpal( Uint8* pal );
void m13h_setfont( fnt_t* fnt );
fnt_t* m13h_getfont( void );
void m13h_clear( void );
void m13h_hscroll( Sint32 offset );
void m13h_vscroll( Sint32 offset );
void m13h_putpixel( Uint16 x, Uint16 y, Uint8 c );
Uint8 m13h_getpixel( Uint16 x, Uint16 y );
void m13h_drawrect( Uint16 x, Uint16 y, Uint16 w, Uint16 h, Uint8 c );
void m13h_drawhline( Uint16 x, Uint16 y, Uint16 l, Uint8 c );
void m13h_drawvline( Uint16 x, Uint16 y, Uint16 l, Uint8 c );
void m13h_drawimg( img_t *img, Uint16 x, Uint16 y, Uint16 ox, Uint16 oy, Uint16 w, Uint16 h, Uint16 palshift );
void m13h_drawchar( Uint16 x, Uint16 y, char c );
void m13h_drawwchar( Uint16 x, Uint16 y, wchar c );
void m13h_drawstring( Uint16 x, Uint16 y, char *s );
void m13h_drawwstring( Uint16 x, Uint16 y, wchar *s );
void m13h_fbgetres( Uint16 *cols, Uint16 *rows );
void m13h_fbgetcursor( Uint16 *col, Uint16 *row );
void m13h_fbsetcursor( Uint16 col, Uint16 row );
void m13h_fbmovecursor( Uint16 cols, Uint16 rows );
void m13h_fbcursorvis( Uint8 on );
void m13h_fbputc( char c );
void m13h_fbwputc( wchar c );
void m13h_fbputs( char *s );
void m13h_fbwputs( wchar *s );
void m13h_fbprintf( char *s, ... );
void m13h_fbwprintf( wchar *s, ... );
void m13h_fbsetattr( Uint8 fg, Uint8 bg, Uint8 ex );
void m13h_fbgetattr( Uint8 *fg, Uint8 *bg, Uint8 *ex );

/* mode 13h struct */
vga_mode_t mode_13h = 
{
	.id = 0x13,
	.name = "Mode 13h",
	.w = 320,
	.h = 200,
	.layout = LAYOUT_LINEAR,
	.depth = DEPTH_256COLOR,
	.mem = (Uint8*)0xA0000,
	.setmode = m13h_setmode,
	.setpal = m13h_setpal,
	.getpal = m13h_setpal,
	.setfont = m13h_setfont,
	.getfont = m13h_getfont,
	.clear = m13h_clear,
	.hscroll = m13h_hscroll,
	.vscroll = m13h_vscroll,
	.putpixel = m13h_putpixel,
	.getpixel = m13h_getpixel,
	.drawrect = m13h_drawrect,
	.drawhline = m13h_drawhline,
	.drawvline = m13h_drawvline,
	.drawimg = m13h_drawimg,
	.drawchar = m13h_drawchar,
	.drawwchar = m13h_drawwchar,
	.drawstring = m13h_drawstring,
	.drawwstring = m13h_drawwstring,
	.fbgetres = m13h_fbgetres,
	.fbgetcursor = m13h_fbgetcursor,
	.fbsetcursor = m13h_fbsetcursor,
	.fbmovecursor = m13h_fbmovecursor,
	.fbcursorvis = m13h_fbcursorvis,
	.fbputc = m13h_fbputc,
	.fbwputc = m13h_fbwputc,
	.fbputs = m13h_fbputs,
	.fbwputs = m13h_fbwputs,
	.fbprintf = m13h_fbprintf,
	.fbwprintf = m13h_fbwprintf,
	.fbsetattr = m13h_fbsetattr,
	.fbgetattr = m13h_fbgetattr,
};

/* mode 13h functions */
void m13h_setmode( void )
{
	int i;
	Uint8 regs[] =
	{
		/* MISC */
		0x63,
		/* SEQ */
		0x03, 0x01, 0x0F, 0x00, 0x0E,
		/* CRTC */
		0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
		0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
		0xFF,
		/* GC */
		0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
		0xFF,
		/* AC */
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
		0x41, 0x00, 0x0F, 0x00, 0x00,
	};
	int rg = 0;
	Uint8 reg;
	/* blank display */
	vga_blank();
	/* MISC */
	outport_b(VGA_MSCOUTW,regs[rg]);
	rg++;
	/* SEQ */
	for ( i=0; i<5; i++ )
		vga_setreg(VGA_SEQ,i,regs[rg++]);
	/* need to unlock CRTC here */
	reg = vga_getreg(VGA_CRTC,VGA_CRTC_EHBLNK);
	reg |= 0x80;
	vga_setreg(VGA_CRTC,VGA_CRTC_EHBLNK,reg);
	reg = vga_getreg(VGA_CRTC,VGA_CRTC_VTRTEN);
	reg &= ~0x80;
	vga_setreg(VGA_CRTC,VGA_CRTC_VTRTEN,reg);
	/* keep 'em unlocked */
	regs[rg+VGA_CRTC_EHBLNK] |= 0x80;
	regs[rg+VGA_CRTC_VTRTEN] &= ~0x80;
	/* CRTC */
	for ( i=0; i<25; i++ )
		vga_setreg(VGA_CRTC,i,regs[rg++]);
	/* GC */
	for ( i=0; i<9; i++ )
		vga_setreg(VGA_GC,i,regs[rg++]);
	/* AC */
	for ( i=0; i<9; i++ )
	{
		inport_b(VGA_INSTAT1);
		outport_b(VGA_AC_I,i);
		outport_b(VGA_AC_W,regs[rg++]);
	}
	/* unblank display and lock palette */
	vga_unblank();
	inport_b(VGA_INSTAT1);
	outport_b(VGA_AC_I,0x20);

	/* clear video memory */
	memset(m13h_mem,0x00,64000);
}

void m13h_setpal( Uint8* pal )
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

void m13h_getpal( Uint8* pal )
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

void m13h_setfont( fnt_t* fnt )
{
	m13h_fnt = *fnt;
}

fnt_t* m13h_getfont( void )
{
	return &m13h_fnt;
}

void m13h_clear( void )
{
	memset(m13h_mem,0x00,64000);
}

void m13h_hscroll( Sint32 offset )
{
	/* noscroll */
	if ( !offset )
		return;
	/* scrolling 320 pixels? that's just like clearing the screen */
	if ( abs(offset) >= 320 )
	{
		memset(m13h_mem,0x00,64000);
		return;
	}
	Uint16 ln;
	if ( offset > 0 )
	{
		for ( ln=0; ln<200; ln++ )
		{
			memmove(m13h_mem+(ln*320),m13h_mem+(ln*320)+offset,320-offset);
			memset(m13h_mem+(ln*320),0x00,offset);
		}
	}
	else
	{
		for ( ln=0; ln<200; ln++ )
		{
			memmove(m13h_mem+(ln*320)+offset,m13h_mem+(ln*320),320-offset);
			memset(m13h_mem+(ln*320)+(320-offset),0x00,offset);
		}
	}
}

void m13h_vscroll( Sint32 offset )
{
	/* noscroll */
	if ( !offset )
		return;
	/* scrolling 200 pixels? that's just like clearing the screen */
	if ( abs(offset) >= 200 )
	{
		memset(m13h_mem,0x00,64000);
		return;
	}
	/* vertical scrolling seems faster in theory */
	if ( offset > 0 )
	{
		memmove(m13h_mem,m13h_mem+(offset*320),320*(200-offset));
		memset(m13h_mem,0x00,offset*320);
	}
	else
	{
		memmove(m13h_mem+(offset*320),m13h_mem,320*(200-offset));
		memset(m13h_mem+(offset*320),0x00,offset*320);
	}
}

void m13h_putpixel( Uint16 x, Uint16 y, Uint8 c )
{
	x %= 320;
	y %= 200;
	m13h_mem[x+y*320] = c;
}

Uint8 m13h_getpixel( Uint16 x, Uint16 y )
{
	x %= 320;
	y %= 200;
	return m13h_mem[x+y*320];
}

void m13h_drawrect( Uint16 x, Uint16 y, Uint16 w, Uint16 h, Uint8 c )
{
	Uint16 px, py;
	Uint16 lx, ly;
	px = x;
	py = y;
	lx = x+w;
	ly = y+h;
	while ( (px < lx) && (py < ly) )
	{
		m13h_mem[px+py*320] = c;
		px++;
		if ( px >= lx )
		{
			px = x;
			py++;
		}
	}
}

void m13h_drawhline( Uint16 x, Uint16 y, Uint16 l, Uint8 c )
{
	Uint16 px;
	Uint16 lx;
	px = x;
	lx = x+l;
	while ( px < lx )
		m13h_mem[(px++)+y*320] = c;
}

void m13h_drawvline( Uint16 x, Uint16 y, Uint16 l, Uint8 c )
{
	Uint16 py;
	Uint16 ly;
	py = y;
	ly = y+l;
	while ( py < ly )
		m13h_mem[x+(py++)*320] = c;
}

void m13h_drawimg( img_t *img, Uint16 x, Uint16 y, Uint16 ox, Uint16 oy, Uint16 w, Uint16 h, Uint16 palshift )
{
	Uint16 px, py;
	Uint16 lx, ly;
	Uint16 iw, ih;
	iw = img->w;
	ih = img->h;
	px = x;
	py = y;
	lx = x+w;
	ly = y+h;
	Uint16 ix, iy;
	ix = ox;
	iy = oy;
	while ( (px < lx) && (py < ly) )
	{
		m13h_mem[px+py*320] = img->data[ix+iy*iw]+palshift;
		ix++;
		if ( ix >= iw )
			ix = ox;
		px++;
		if ( px >= lx )
		{
			ix = ox;
			px = x;
			py++;
			iy++;
		}
		if ( iy >= ih )
			iy = oy;
	}
}

void m13h_drawchar( Uint16 x, Uint16 y, char c )
{
	Uint16 px, py;
	Uint16 lx, ly;
	Uint8 cw, ch;
	cw = m13h_fnt.w;
	ch = m13h_fnt.h;
	px = x;
	py = y;
	lx = x+cw;
	ly = y+ch;
	Uint16 off = c*cw*ch;
	Uint16 cx, cy;
	cx = 0;
	cy = 0;
	while ( (px < lx) && (py < ly) )
	{
		m13h_mem[px+py*320] = m13h_fnt.data[off+cx+cy*cw];
		cx++;
		if ( cx >= cw )
		{
			cx = 0;
			cy++;
		}
		px++;
		if ( px >= lx )
		{
			px = x;
			py++;
		}
	}
}

void m13h_drawwchar( Uint16 x, Uint16 y, wchar c )
{
	Uint16 px, py;
	Uint16 lx, ly;
	Uint8 cw, ch;
	cw = m13h_fnt.w;
	ch = m13h_fnt.h;
	px = x;
	py = y;
	lx = x+cw;
	ly = y+ch;
	Uint32 off = c*cw*ch;
	Uint16 cx, cy;
	cx = 0;
	cy = 0;
	while ( (px < lx) && (py < ly) )
	{
		m13h_mem[px+py*320] = m13h_fnt.data[off+cx+cy*cw];
		cx++;
		if ( cx >= cw )
		{
			cx = 0;
			cy++;
		}
		px++;
		if ( px >= lx )
		{
			px = x;
			py++;
		}
	}
}

void m13h_drawstring( Uint16 x, Uint16 y, char *s )
{
	while ( *s )
	{
		m13h_drawchar(x,y,*(s++));
		x += m13h_fnt.w;
	}
}

void m13h_drawwstring( Uint16 x, Uint16 y, wchar *s )
{
	while ( *s )
	{
		m13h_drawwchar(x,y,*(s++));
		x += m13h_fnt.w;
	}
}

void m13h_fbgetres( Uint16 *cols, Uint16 *rows )
{
	*cols = m13h_fbw;
	*rows = m13h_fbh;
}

void m13h_fbgetcursor( Uint16 *col, Uint16 *row )
{
	*col = m13h_cx;
	*row = m13h_cy;
}

void m13h_fbsetcursor( Uint16 col, Uint16 row )
{
	col %= m13h_fbw;
	row %= m13h_fbh;
	m13h_cx = col;
	m13h_cy = row;
}

void m13h_fbmovecursor( Uint16 cols, Uint16 rows )
{
	m13h_cx += cols;
	m13h_cy += rows;
	m13h_cx %= m13h_fbw;
	m13h_cy %= m13h_fbh;
}

void m13h_fbcursorvis( Uint8 on )
{
	m13h_cv = on&1;
}

void m13h_fbputc( char c )
{
	return;	/* not yet implemented */
}

void m13h_fbwputc( wchar c )
{
	return;	/* not yet implemented */
}

void m13h_fbputs( char *s )
{
	return;	/* not yet implemented */
}

void m13h_fbwputs( wchar *s )
{
	return;	/* not yet implemented */
}

void m13h_fbprintf( char *s, ... )
{
	return;	/* not yet implemented */
}

void m13h_fbwprintf( wchar *s, ... )
{
	return;	/* not yet implemented */
}

void m13h_fbsetattr( Uint8 fg, Uint8 bg, Uint8 ex )
{
	m13h_attrs[0] = fg;
	m13h_attrs[1] = bg;
	m13h_attrs[2] = ex;
}

void m13h_fbgetattr( Uint8 *fg, Uint8 *bg, Uint8 *ex )
{
	*fg = m13h_attrs[0];
	*bg = m13h_attrs[1];
	*ex = m13h_attrs[2];
}

