/*
	vgamode12h.c : Mode 12h (planar 640x480 16 colors).
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/port.h>
#include <sys/helpers.h>
#include <sys/va_list.h>
#include <vga/mode12h.h>
#include <vga/vgareg.h>
#include <vga/vgamisc.h>
#include <memops.h>

/* mode 12h variables */
static Uint8 *m12h_mem = (Uint8*)0xA0000; /* memory area */
static fnt_t m12h_fnt; /* font (currently empty) */
static Sint32 m12h_cx = 0, m12h_cy = 0; /* cursor position for text */
static Uint8 m12h_cv = 1; /* cursor visibility for text */
static Uint16 m12h_fbw = 80, m12h_fbh = 60; /* framebuffer console columns and rows */
static Uint8 m12h_attrs[3] = {7,0,0}; /* current text attributes */

/* mode 12h function prototypes */
static void m12h_setmode( void );
static void m12h_setpal( Uint8* pal );
static void m12h_getpal( Uint8* pal );
static void m12h_setfont( fnt_t* fnt );
static fnt_t* m12h_getfont( void );
static void m12h_clear( void );
static void m12h_hscroll( Sint32 offset );
static void m12h_vscroll( Sint32 offset );
static void m12h_putpixel( Uint16 x, Uint16 y, Uint8 c );
static Uint8 m12h_getpixel( Uint16 x, Uint16 y );
static void m12h_drawrect( Uint16 x, Uint16 y, Uint16 w, Uint16 h, Uint8 c );
static void m12h_drawhline( Uint16 x, Uint16 y, Uint16 l, Uint8 c );
static void m12h_drawvline( Uint16 x, Uint16 y, Uint16 l, Uint8 c );
static void m12h_drawimg( img_t *img, Uint16 x, Uint16 y, Uint16 ox, Uint16 oy, Uint16 w, Uint16 h, Uint16 palshift );
static void m12h_drawchar( Uint16 x, Uint16 y, char c );
static void m12h_drawwchar( Uint16 x, Uint16 y, wchar c );
static void m12h_drawstring( Uint16 x, Uint16 y, char *s );
static void m12h_drawwstring( Uint16 x, Uint16 y, wchar *s );
static void m12h_fbgetres( Uint16 *cols, Uint16 *rows );
static void m12h_fbgetcursor( Sint32 *col, Sint32 *row );
static void m12h_fbsetcursor( Sint32 col, Sint32 row );
static void m12h_fbmovecursor( Sint32 cols, Sint32 rows );
static void m12h_fbcursorvis( Uint8 on );
static void m12h_fbputc( char c );
static void m12h_fbwputc( wchar c );
static void m12h_fbputs( char *s );
static void m12h_fbwputs( wchar *s );
static void m12h_fbprintf( char *s, ... );
static void m12h_fbwprintf( wchar *s, ... );
static void m12h_fbsetattr( Uint8 fg, Uint8 bg, Uint8 ex );
static void m12h_fbgetattr( Uint8 *fg, Uint8 *bg, Uint8 *ex );

/* mode 12h struct */
vga_mode_t mode_12h = 
{
	.id = 0x12,
	.name = "Mode 12h",
	.w = 640,
	.h = 480,
	.layout = LAYOUT_PLANAR,
	.depth = DEPTH_16COLOR,
	.mem = (Uint8*)0xA0000,
	.setmode = m12h_setmode,
	.setpal = m12h_setpal,
	.getpal = m12h_setpal,
	.setfont = m12h_setfont,
	.getfont = m12h_getfont,
	.clear = m12h_clear,
	.hscroll = m12h_hscroll,
	.vscroll = m12h_vscroll,
	.putpixel = m12h_putpixel,
	.getpixel = m12h_getpixel,
	.drawrect = m12h_drawrect,
	.drawhline = m12h_drawhline,
	.drawvline = m12h_drawvline,
	.drawimg = m12h_drawimg,
	.drawchar = m12h_drawchar,
	.drawwchar = m12h_drawwchar,
	.drawstring = m12h_drawstring,
	.drawwstring = m12h_drawwstring,
	.fbgetres = m12h_fbgetres,
	.fbgetcursor = m12h_fbgetcursor,
	.fbsetcursor = m12h_fbsetcursor,
	.fbmovecursor = m12h_fbmovecursor,
	.fbcursorvis = m12h_fbcursorvis,
	.fbputc = m12h_fbputc,
	.fbwputc = m12h_fbwputc,
	.fbputs = m12h_fbputs,
	.fbwputs = m12h_fbwputs,
	.fbprintf = m12h_fbprintf,
	.fbwprintf = m12h_fbwprintf,
	.fbsetattr = m12h_fbsetattr,
	.fbgetattr = m12h_fbgetattr,
};

static void m12h_setmode( void )
{
	int i;
	Uint8 regs[] =
	{
		/* MISC */
		0xE3,
		/* SEQ */
		0x03, 0x01, 0x08, 0x00, 0x06,
		/* CRTC */
		0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0x0B, 0x3E,
		0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xEA, 0x0C, 0xDF, 0x28, 0x00, 0xE7, 0x04, 0xE3,
		0xFF,
		/* GC */
		0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x05, 0x0F,
		0xFF,
		/* AC */
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
		0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
		0x01, 0x00, 0x0F, 0x00, 0x00,
	};
	int rg = 0;
	Uint8 reg;
	/* blank display */
	vgablank();
	/* MISC */
	outport_b(VGA_MSCOUTW,regs[rg]);
	rg++;
	/* SEQ */
	for ( i=0; i<5; i++ )
		setvgareg(VGA_SEQ,i,regs[rg++]);
	/* need to unlock CRTC here */
	reg = getvgareg(VGA_CRTC,VGA_CRTC_EHBLNK);
	reg |= 0x80;
	setvgareg(VGA_CRTC,VGA_CRTC_EHBLNK,reg);
	reg = getvgareg(VGA_CRTC,VGA_CRTC_VTRTEN);
	reg &= ~0x80;
	setvgareg(VGA_CRTC,VGA_CRTC_VTRTEN,reg);
	/* keep 'em unlocked */
	regs[rg+VGA_CRTC_EHBLNK] |= 0x80;
	regs[rg+VGA_CRTC_VTRTEN] &= ~0x80;
	/* CRTC */
	for ( i=0; i<25; i++ )
		setvgareg(VGA_CRTC,i,regs[rg++]);
	/* GC */
	for ( i=0; i<9; i++ )
		setvgareg(VGA_GC,i,regs[rg++]);
	/* AC */
	for ( i=0; i<9; i++ )
	{
		inport_b(VGA_INSTAT1);
		outport_b(VGA_AC_I,i);
		outport_b(VGA_AC_W,regs[rg++]);
	}
	/* unblank display and lock palette */
	vgaunblank();
	inport_b(VGA_INSTAT1);
	outport_b(VGA_AC_I,0x20);
	/* clear video memory */
	Uint8 p;
	for ( p=0; p<4; p++ )
	{
		planeswitch(p);
		memset(m12h_mem,0x00,38400);
	}
}

static void m12h_setpal( Uint8* pal )
{
	int i;
	/* CGA->EGA mapping */
	int map[16] = {0,1,2,3,4,5,20,7,56,57,58,59,60,61,62,63};
	for ( i=0; i<16; i++ )
	{
		outport_b(VGA_DAC_ADWM,map[i]);
		outport_b(VGA_DAC_DATA,(*pal++));
		outport_b(VGA_DAC_DATA,(*pal++));
		outport_b(VGA_DAC_DATA,(*pal++));
	}
}

static void m12h_getpal( Uint8* pal )
{
	int i;
	/* CGA->EGA mapping */
	int map[16] = {0,1,2,3,4,5,20,7,56,57,58,59,60,61,62,63};
	for ( i=0; i<16; i++ )
	{
		outport_b(VGA_DAC_ADRM,map[i]);
		*(pal++) = inport_b(VGA_DAC_DATA);
		*(pal++) = inport_b(VGA_DAC_DATA);
		*(pal++) = inport_b(VGA_DAC_DATA);
	}
}

static void m12h_setfont( fnt_t* fnt )
{
	m12h_fnt = *fnt;
}

static fnt_t* m12h_getfont( void )
{
	return &m12h_fnt;
}

static void m12h_clear( void )
{
	Uint8 p;
	for ( p=0; p<4; p++ )
	{
		planeswitch(p);
		memset(m12h_mem,0x00,38400);
	}
}

static void m12h_hscroll( Sint32 offset )
{
	offset /= 8;	/* deal with it */
	Uint8 p;
	/* noscroll */
	if ( !offset )
		return;
	/* scrolling 80 blocks? that's just like clearing the screen */
	if ( abs(offset) >= 80 )
	{
		for ( p=0; p<4; p++ )
		{
			planeswitch(p);
			memset(m12h_mem,0x00,38400);
		}
		return;
	}
	Uint16 ln;
	if ( offset > 0 )
	{
		for ( p=0; p<4; p++ )
		{
			planeswitch(p);
			for ( ln=0; ln<480; ln++ )
			{
				memmove(m12h_mem+(ln*80)+offset,m12h_mem+(ln*80),80-offset);
				memset(m12h_mem+(ln*80),0x00,offset);
			}
		}
	}
	else
	{
		offset *= -1;
		for ( p=0; p<4; p++ )
		{
			planeswitch(p);
			for ( ln=0; ln<480; ln++ )
			{
				memmove(m12h_mem+(ln*80),m12h_mem+(ln*80)+offset,80-offset);
				memset(m12h_mem+(ln*80)+(80-offset),0x00,offset);
			}
		}
	}
}

static void m12h_vscroll( Sint32 offset )
{
	Uint8 p;
	/* noscroll */
	if ( !offset )
		return;
	/* scrolling 480 pixels? that's just like clearing the screen */
	if ( abs(offset) >= 480 )
	{
		for ( p=0; p<4; p++ )
		{
			planeswitch(p);
			memset(m12h_mem,0x00,38400);
		}
		return;
	}
	/* vertical scrolling seems faster in theory */
	if ( offset > 0 )
	{
		for ( p=0; p<4; p++ )
		{
			planeswitch(p);
			memmove(m12h_mem+(offset*80),m12h_mem,80*(480-offset));
			memset(m12h_mem,0x00,offset*80);
		}
	}
	else
	{
		offset *= -1;
		for ( p=0; p<4; p++ )
		{
			planeswitch(p);
			memmove(m12h_mem,m12h_mem+(offset*80),80*(480-offset));
			memset(m12h_mem+((480-offset)*80),0x00,offset*80);
		}
	}
}

static void m12h_putpixel( Uint16 x, Uint16 y, Uint8 c )
{
	x %= 640;
	y %= 480;
	c &= 15;
	Uint8 p;
	Uint16 coord = (x+y*640)/8;
	Uint8 off = 7-(x%8);
	Uint8 c2[4] = { c&1, (c>>1)&1, (c>>2)&1, (c>>3)&1 };
	for ( p=0; p<4; p++ )
	{
		planeswitch(p);
		m12h_mem[coord] &= ~(1<<off);
		m12h_mem[coord] |= c2[p]<<off;
	}
}

static Uint8 m12h_getpixel( Uint16 x, Uint16 y )
{
	x %= 640;
	y %= 480;
	Uint8 p;
	Uint16 coord = (x+y*640)/8;
	Uint8 off = 7-(x%8);
	Uint8 c2[4];
	for ( p=0; p<4; p++ )
	{
		planeswitch(p);
		c2[p] = (m12h_mem[coord]>>off)&1;
	}
	return c2[0]&(c2[1]<<1)&(c2[2]<<2)&(c2[3]<<4);
}

static void m12h_putpixelsp( Uint16 x, Uint16 y, Uint8 c )
{
	x %= 640;
	y %= 480;
	c &= 1;
	Uint16 coord = (x+y*640)/8;
	Uint8 off = 7-(x%8);
	m12h_mem[coord] &= ~(1<<off);
	m12h_mem[coord] |= c<<off;
}

static Uint8 m12h_getpixelsp( Uint16 x, Uint16 y )
{
	x %= 640;
	y %= 480;
	Uint16 coord = (x+y*640)/8;
	Uint8 off = 7-(x%8);
	return (m12h_mem[coord]>>off)&1;
}

static void m12h_drawrect( Uint16 x, Uint16 y, Uint16 w, Uint16 h, Uint8 c )
{
	Uint8 p;
	Uint16 px, py;
	Uint16 lx, ly;
	for ( p=0; p<4; p++ )
	{
		planeswitch(p);
		px = x;
		py = y;
		lx = x+w;
		ly = y+h;
		while ( (px < lx) && (py < ly) )
		{
			m12h_putpixelsp(px,py,(c>>p)&1);
			px++;
			py = (px>=lx)?(py+1):py;
			px = (px>=lx)?x:px;
		}
	}
}

static void m12h_drawhline( Uint16 x, Uint16 y, Uint16 l, Uint8 c )
{
	Uint8 p;
	Uint16 px;
	Uint16 lx;
	for ( p=0; p<4; p++ )
	{
		planeswitch(p);
		px = x;
		lx = x+l;
		while ( px < lx )
		{
			m12h_putpixelsp(px,y,(c>>p)&1);
			px++;
		}
	}
}

static void m12h_drawvline( Uint16 x, Uint16 y, Uint16 l, Uint8 c )
{
	Uint8 p;
	Uint16 py;
	Uint16 ly;
	for ( p=0; p<4; p++ )
	{
		planeswitch(p);
		py = y;
		ly = y+l;
		while ( py < ly )
		{
			m12h_putpixelsp(x,py,(c>>p)&1);
			py++;
		}
	}
}

static void m12h_drawimg( img_t *img, Uint16 x, Uint16 y, Uint16 ox, Uint16 oy, Uint16 w, Uint16 h, Uint16 palshift )
{
	if ( img->depth != IMG_DP_16COL )
	{
		/* not supported */
		m12h_drawrect(x,y,w,h,1);
		return;
	}
	Uint16 px, py;
	Uint16 lx, ly;
	Uint16 iw, ih;
	Uint16 ix, iy;
	Uint8 c;
	Uint8 p;
	for ( p=0; p<4; p++ )
	{
		iw = img->w;
		ih = img->h;
		px = x;
		py = y;
		lx = x+w;
		ly = y+h;
		ix = ox;
		iy = oy;
		planeswitch(p);
		while ( (px < lx) && (py < ly) )
		{
			c = img->data[ix+iy*iw];
			if ( c <= 15 )
			{
				c += palshift;
				m12h_putpixelsp(px,py,(c>>p)&1);
			}
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
}

static void m12h_drawchar( Uint16 x, Uint16 y, char c )
{
	Uint16 px, py;
	Uint16 lx, ly;
	Uint8 cw, ch;
	Uint16 off;
	Uint16 cx, cy;
	Uint8 p;
	for ( p=0; p<4; p++ )
	{
		planeswitch(p);
		cw = m12h_fnt.w;
		ch = m12h_fnt.h;
		px = x;
		py = y;
		lx = x+cw;
		ly = y+ch;
		off = c*cw*ch;
		cx = 0;
		cy = 0;
		while ( (px < lx) && (py < ly) )
		{
			Uint8 c2;
			Uint16 cx2, cy2;
			Uint16 ctmp;
			cx2 = cx;
			cy2 = cy;
			if ( m12h_attrs[2]&EXATTR_HFLIP )
				cx2 = (cw-1)-cx;
			if ( m12h_attrs[2]&EXATTR_VFLIP )
				cy2 = (ch-1)-cy;
			if ( m12h_attrs[2]&EXATTR_RCW90 )
			{
				ctmp = (cw-1)-cx2;
				cx2 = cy2;
				cy2 = ctmp;
			}
			if ( m12h_attrs[2]&EXATTR_RCCW90 )
			{
				ctmp = (ch-1)-cy2;
				cy2 = cx2;
				cx2 = ctmp;
			}
			c2 = m12h_attrs[1];
			if ( m12h_fnt.data[off+cx2+cy2*cw] )
				c2 = (m12h_attrs[2]&EXATTR_REVBG)?(15-c):m12h_attrs[0];
			else if ( m12h_attrs[2]&EXATTR_MASKED && !m12h_attrs[1] )
			{
				c2 = m12h_getpixelsp(px,py);
				c2 <<=p;
			}
			m12h_putpixelsp(px,py,(((m12h_attrs[2]&EXATTR_NODW)?m12h_attrs[1]:c2)>>p)&1);
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
}

static void m12h_drawwchar( Uint16 x, Uint16 y, wchar c )
{
	return;	/* not yet implemented */
}

static void m12h_drawstring( Uint16 x, Uint16 y, char *s )
{
	while ( *s )
	{
		m12h_drawchar(x,y,*(s++));
		x += m12h_fnt.w;
	}
}

static void m12h_drawwstring( Uint16 x, Uint16 y, wchar *s )
{
	return;	/* not yet implemented */
}

static void m12h_fbgetres( Uint16 *cols, Uint16 *rows )
{
	*cols = m12h_fbw;
	*rows = m12h_fbh;
}

static void m12h_fbgetcursor( Sint32 *col, Sint32 *row )
{
	*col = m12h_cx;
	*row = m12h_cy;
}

static void m12h_fbsetcursor( Sint32 col, Sint32 row )
{
	while ( col >= m12h_fbw )
		col -= m12h_fbw;
	while ( col < 0 )
		col += m12h_fbw;
	while ( row >= m12h_fbh )
		row -= m12h_fbh;
	while ( row < 0 )
		row += m12h_fbh;
	m12h_cx = col;
	m12h_cy = row;
}

static void m12h_fbmovecursor( Sint32 cols, Sint32 rows )
{
	Sint32 px, py;
	px = m12h_cx+cols;
	py = m12h_cy+rows;
	while ( px >= m12h_fbw )
		px -= m12h_fbw;
	while ( px < 0 )
		px += m12h_fbw;
	while ( py >= m12h_fbh )
		py -= m12h_fbh;
	while ( py < 0 )
		py += m12h_fbh;
	m12h_cx += px;
	m12h_cy += py;
}

static void m12h_fbcursorvis( Uint8 on )
{
	m12h_cv = on&1;
}

static void m12h_fbputc( char c )
{
	if ( m12h_cy >= m12h_fbh )	/* offscreen */
		return;
	if ( c == '\b' )
		m12h_cx -= (m12h_cx!=0)?1:0;
	else if ( c == '\t' )
		m12h_cx = (m12h_cx+8)&~(8-1);
	else if ( c == '\r' )
		m12h_cx = 0;
	else if ( c == '\n' )
	{
		m12h_cx = 0;
		m12h_cy++;
	}
	else
	{
		m12h_drawchar(m12h_cx*m12h_fnt.w,m12h_cy*m12h_fnt.h,c);
		m12h_cx++;
	}
	if ( m12h_cx >= m12h_fbw )
	{
		m12h_cx = 0;
		m12h_cy++;
	}
	if ( (m12h_cy >= m12h_fbh) && !(m12h_attrs[2]&EXATTR_NOSCR) )
	{
		m12h_vscroll(m12h_fnt.h*(-1));
		m12h_cy--;
	}
}

static void m12h_fbwputc( wchar c )
{
	return;	/* not yet implemented */
}

static void m12h_fbputs( char *s )
{
	while ( *s )
		m12h_fbputc(*(s++));
}

static void m12h_fbwputs( wchar *s )
{
	return;	/* not yet implemented */
}

static Uint32 m12h_vafbprintf_sattr( char *s, Uint8 ofg, Uint8 obg, Uint8 oex )
{
	char *os = s;
	Uint8 col = obg;
	Uint8 nex = oex;
	Uint8 sh = 0;
	if ( ((*s >= '0') && (*s <= '9')) || ((*s >= 'A') && (*s <= 'F')) || (*s == '-') )
	{
		if ( *s == '-' )
			col = ((col&0x0F)<<4)|obg;
		else
			col = ((col&0x0F)<<4)|(((*s <= '9')?(*s-'0'):((*s+0xA)-'A'))&0x0F);
		s++;
	}
	if ( ((*s >= '0') && (*s <= '9')) || ((*s >= 'A') && (*s <= 'F')) || (*s == '-') )
	{
		if ( *s == '-' )
			col = ((col&0x0F)<<4)|ofg;
		else
			col = ((col&0x0F)<<4)|(((*s <= '9')?(*s-'0'):((*s+0xA)-'A'))&0x0F);
		s++;
	}
	while ( (((*s >= '0') && (*s <= '1')) || (*s == '-')) && (sh < 8) )
	{
		nex &= ~(1<<sh);
		if ( *s == '-' )
			nex |= oex&(1<<sh);
		else
			nex |= (*s-'0')<<sh;
		s++;
		sh++;
	}
	m12h_fbsetattr(col&0x0F,(col>>4)&0x0F,nex);
	return (Uint32)s-(Uint32)os;
}

static Uint32 m12h_vafbprintf_curmv( char *s )
{
	char *os = s;
	Uint8 neg = 0;
	Sint32 x = 0;
	Sint32 y = 0;
	if ( *s == '-' )
	{
		neg = 1;
		s++;
	}
	while ( (*s >= '0') && (*s <= '9') )
		x = x*10+(*(s++)-'0');
	if ( neg )
		x *= -1;
	neg = 0;
	if ( *s == ',' )
	{
		s++;
		if ( *s == '-' )
		{
			neg = 1;
			s++;
		}
		while ( (*s >= '0') && (*s <= '9') )
			y = y*10+(*(s++)-'0');
		if ( neg )
			y *= -1;
	}
	m12h_fbmovecursor(x,y);
	return (Uint32)s-(Uint32)os;
}

static Uint32 m12h_vafbprintf_curset( char *s, Sint32 y )
{
	char *os = s;
	Uint8 neg = 0;
	Sint32 x = 0;
	if ( *s == '-' )
	{
		neg = 1;
		s++;
	}
	while ( (*s >= '0') && (*s <= '9') )
		x = x*10+(*(s++)-'0');
	if ( neg )
		x *= -1;
	neg = 0;
	if ( *s == ',' )
	{
		y = 0;
		s++;
		if ( *s == '-' )
		{
			neg = 1;
			s++;
		}
		while ( (*s >= '0') && (*s <= '9') )
			y = y*10+(*(s++)-'0');
		if ( neg )
			y *= -1;
	}
	m12h_fbsetcursor(x,y);
	return (Uint32)s-(Uint32)os;
}

static void m12h_fbputu( Uint32 val, Uint16 width, Uint8 zeropad )
{
	if ( !width )
	{
		char c[10];
		Sint32 i = 0;
		do
		{
			c[i++] = '0'+val%10;
			val /= 10;
		}
		while ( val != 0 );
		while ( i > 0 )
			m12h_fbputc(c[--i]);
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
			m12h_fbputc(c[--i]);
	}
}

static void m12h_fbputd( Sint32 val, Uint16 width, Uint8 zeropad )
{
	Uint8 isneg = (val<0);
	val = abs(val);
	if ( !width )
	{
		char c[10];
		Sint32 i = 0;
		do
		{
			c[i++] = '0'+val%10;
			val /= 10;
		}
		while ( val != 0 );
		if ( isneg )
			m12h_fbputc('-');
		while ( i > 0 )
			m12h_fbputc(c[--i]);
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
		if ( isneg )
			m12h_fbputc('-');
		while ( i < width )
			c[i++] = (zeropad)?'0':' ';
		while ( i > 0 )
			m12h_fbputc(c[--i]);
	}
}

static void m12h_fbputh( Uint32 val, Uint16 width, Uint8 zeropad )
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
			m12h_fbputc(c[--i]);
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
			m12h_fbputc(c[--i]);
	}
}

static void m12h_fbputo( Uint32 val, Uint16 width, Uint8 zeropad )
{
	if ( !width )
	{
		char c[11];
		Sint32 i = 0;
		do
		{
			c[i++] = '0'+(val&0x07);
			val >>= 3;
		}
		while ( val != 0 );
		while ( i > 0 )
			m12h_fbputc(c[--i]);
	}
	else
	{
		char c[width];
		Sint32 i = 0;
		do
		{
			c[i++] = '0'+(val&0x07);
			val >>= 3;
		}
		while ( (val != 0) && (i < width) );
		while ( i < width )
			c[i++] = (zeropad)?'0':' ';
		while ( i > 0 )
			m12h_fbputc(c[--i]);
	}
}

static void m12h_vafbprintf( char *s, va_list args )
{
	Uint8 fg, bg, ex;
	Sint32 x, y;
	Uint8 alt;
	Uint8 zp;
	Uint16 wide;
	/* base loop */
	while ( *s )
	{
		x = 0;
		y = 0;
		m12h_fbgetattr(&fg,&bg,&ex);
		m12h_fbgetcursor(&x,&y);
		alt = 0;
		zp = 0;
		wide = 0;
		if ( *s != '%' )	/* not an escape, print normally */
		{
			m12h_fbputc(*(s++));
			continue;
		}
		s++;
		if ( *s == '%' )	/* just a percent sign */
		{
			m12h_fbputc(*(s++));
			continue;
		}
		if ( *s == 's' )	/* string */
		{
			m12h_fbputs((char*)va_arg(args,char*));
			s++;
			continue;
		}
		if ( *s == 'S' )	/* wstring */
		{
			m12h_fbwputs((wchar*)va_arg(args,unsigned long*));
			s++;
			continue;
		}
		if ( *s == 'c' )	/* char */
		{
			m12h_fbputc((char)va_arg(args,int));
			s++;
			continue;
		}
		if ( *s == 'C' )	/* wchar */
		{
			m12h_fbwputc((wchar)va_arg(args,unsigned long));
			s++;
			continue;
		}
		if ( *s == '#' )	/* alternate form */
		{
			alt = 1;
			s++;
		}
		if ( *s == '0' )	/* zero padding */
		{
			zp = 1;
			s++;
		}
		if ( *s == '[' )	/* attribute change */
		{
			Uint32 skip = m12h_vafbprintf_sattr(++s,fg,bg,ex);
			s += skip;
			continue;
		}
		if ( *s == '(' )	/* cursor move */
		{
			Uint32 skip = m12h_vafbprintf_curmv(++s);
			s += skip;
			continue;
		}
		if ( *s == '{' )	/* cursor set */
		{
			Uint32 skip = m12h_vafbprintf_curset(++s,y);
			s += skip;
			continue;
		}
		while ( (*s >= '0') && (*s <= '9') )	/* field width */
			wide = wide*10+(*(s++)-'0');
		if ( *s == 'd' )	/* signed base 10 integer */
		{
			m12h_fbputd((signed long)va_arg(args,signed long),wide,zp);
			s++;
			continue;
		}
		if ( *s == 'u' )	/* unsigned base 10 integer */
		{
			m12h_fbputu((unsigned long)va_arg(args,unsigned long),wide,zp);
			s++;
			continue;
		}
		if ( *s == 'x' )	/* unsigned base 16 integer */
		{
			if ( alt )
				m12h_fbputs("0x");
			m12h_fbputh((unsigned long)va_arg(args,unsigned long),wide,zp);
			s++;
			continue;
		}
		if ( *s == 'o' )	/* unsigned base 8 integer */
		{
			if ( alt )
				m12h_fbputc('0');
			m12h_fbputo((unsigned long)va_arg(args,unsigned long),wide,zp);
			s++;
			continue;
		}
		/* meh */
		m12h_fbputc(*(s++));
	}
}

static void m12h_fbprintf( char *s, ... )
{
	va_list args;
	va_start(args,s);
	m12h_vafbprintf(s,args);
	va_end(args);
}

static void m12h_fbwprintf( wchar *s, ... )
{
	return;	/* not yet implemented */
}

static void m12h_fbsetattr( Uint8 fg, Uint8 bg, Uint8 ex )
{
	m12h_attrs[0] = fg;
	m12h_attrs[1] = bg;
	m12h_attrs[2] = ex;
}

static void m12h_fbgetattr( Uint8 *fg, Uint8 *bg, Uint8 *ex )
{
	*fg = m12h_attrs[0];
	*bg = m12h_attrs[1];
	*ex = m12h_attrs[2];
}
