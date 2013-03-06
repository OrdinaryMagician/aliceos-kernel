/*
	mode3h.c : Mode 3h (80x25 16-color text mode).
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/port.h>
#include <sys/helpers.h>
#include <sys/va_list.h>
#include <vga/mode13h.h>
#include <vga/vgareg.h>
#include <vga/vgamisc.h>
#include <vga/vgafont.h>
#include <memops.h>

/* mode 3h variables */
static Uint16 *m3h_mem = (Uint16*)0xB8000; /* memory area */
static fnt_t m3h_fnt; /* font (currently empty) */
static Sint32 m3h_cx = 0, m3h_cy = 0; /* cursor position for text */
static Uint8 m3h_cv = 1; /* cursor visibility for text */
static Uint16 m3h_fbw = 80, m3h_fbh = 25; /* framebuffer console columns and rows */
static Uint8 m3h_attrs[3] = {7,0,0}; /* current text attributes */

/* mode 3h function prototypes */
static void m3h_setmode( void );
static void m3h_setpal( Uint8* pal );
static void m3h_getpal( Uint8* pal );
static void m3h_setfont( fnt_t* fnt );
static fnt_t* m3h_getfont( void );
static void m3h_clear( void );
static void m3h_hscroll( Sint32 offset );
static void m3h_vscroll( Sint32 offset );
static void m3h_putpixel( Uint16 x, Uint16 y, Uint8 c );
static Uint8 m3h_getpixel( Uint16 x, Uint16 y );
static void m3h_drawrect( Uint16 x, Uint16 y, Uint16 w, Uint16 h, Uint8 c );
static void m3h_drawhline( Uint16 x, Uint16 y, Uint16 l, Uint8 c );
static void m3h_drawvline( Uint16 x, Uint16 y, Uint16 l, Uint8 c );
static void m3h_drawimg( img_t *img, Uint16 x, Uint16 y, Uint16 ox, Uint16 oy, Uint16 w, Uint16 h, Uint16 palshift );
static void m3h_drawchar( Uint16 x, Uint16 y, char c );
static void m3h_drawwchar( Uint16 x, Uint16 y, wchar c );
static void m3h_drawstring( Uint16 x, Uint16 y, char *s );
static void m3h_drawwstring( Uint16 x, Uint16 y, wchar *s );
static void m3h_fbgetres( Uint16 *cols, Uint16 *rows );
static void m3h_fbgetcursor( Sint32 *col, Sint32 *row );
static void m3h_fbsetcursor( Sint32 col, Sint32 row );
static void m3h_fbmovecursor( Sint32 cols, Sint32 rows );
static void m3h_fbcursorvis( Uint8 on );
static void m3h_fbputc( char c );
static void m3h_fbwputc( wchar c );
static void m3h_fbputs( char *s );
static void m3h_fbwputs( wchar *s );
static void m3h_fbprintf( char *s, ... );
static void m3h_fbwprintf( wchar *s, ... );
static void m3h_fbsetattr( Uint8 fg, Uint8 bg, Uint8 ex );
static void m3h_fbgetattr( Uint8 *fg, Uint8 *bg, Uint8 *ex );

/* mode 3h struct */
vga_mode_t mode_3h =
{
	.id = 0x03,
	.name = "Mode 3h",
	.w = 80,
	.h = 50,
	.layout = LAYOUT_TEXT,
	.depth = DEPTH_16COLOR,
	.mem = (Uint8*)0xB8000,
	.setmode = m3h_setmode,
	.setpal = m3h_setpal,
	.getpal = m3h_setpal,
	.setfont = m3h_setfont,
	.getfont = m3h_getfont,
	.clear = m3h_clear,
	.hscroll = m3h_hscroll,
	.vscroll = m3h_vscroll,
	.putpixel = m3h_putpixel,
	.getpixel = m3h_getpixel,
	.drawrect = m3h_drawrect,
	.drawhline = m3h_drawhline,
	.drawvline = m3h_drawvline,
	.drawimg = m3h_drawimg,
	.drawchar = m3h_drawchar,
	.drawwchar = m3h_drawwchar,
	.drawstring = m3h_drawstring,
	.drawwstring = m3h_drawwstring,
	.fbgetres = m3h_fbgetres,
	.fbgetcursor = m3h_fbgetcursor,
	.fbsetcursor = m3h_fbsetcursor,
	.fbmovecursor = m3h_fbmovecursor,
	.fbcursorvis = m3h_fbcursorvis,
	.fbputc = m3h_fbputc,
	.fbwputc = m3h_fbwputc,
	.fbputs = m3h_fbputs,
	.fbwputs = m3h_fbwputs,
	.fbprintf = m3h_fbprintf,
	.fbwprintf = m3h_fbwprintf,
	.fbsetattr = m3h_fbsetattr,
	.fbgetattr = m3h_fbgetattr,
};


/* mode 3h functions */
static void m3h_setmode( void )
{
	int i;
	Uint8 regs[] =
	{
		/* MISC */
		0x63,
		/* SEQ */
		0x03, 0x01, 0x03, 0x00, 0x02,
		/* CRTC */
		0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F,
		0x00, 0x4F, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x50,
		0x9C, 0x0E, 0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3,
		0xFF,
		/* GC */
		0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00,
		0xFF,
		/* AC */
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
		0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
		0x0C, 0x00, 0x0F, 0x08, 0x00,
	};
	int rg = 0;
	Uint8 reg;
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
	inport_b(VGA_INSTAT1);
	outport_b(VGA_AC_I,0x20);
	/* clear video memory */
	memsetw(m3h_mem,0x00,2000);
}

static void m3h_setpal( Uint8* pal )
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

static void m3h_getpal( Uint8* pal )
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

static void m3h_setfont( fnt_t* fnt )
{
	m3h_fnt = *fnt;
	setfont_256(m3h_fnt.data);
}

static fnt_t* m3h_getfont( void )
{
	getfont_256(m3h_fnt.data);
	return &m3h_fnt;
}

static void m3h_clear( void )
{
	memsetw(m3h_mem,0x0F00,2000);
}

static void m3h_hscroll( Sint32 offset )
{
	/* noscroll */
	if ( !offset )
		return;
	/* scrolling m3h_fbw blocks? that's just like clearing the screen */
	if ( abs(offset) >= m3h_fbw )
	{
		memsetw(m3h_mem,0,2000);
		return;
	}
	Uint16 ln;
	if ( offset > 0 )
	{
		for ( ln=0; ln<m3h_fbh; ln++ )
		{
			memmovew(m3h_mem+(ln*m3h_fbw)+offset,m3h_mem+(ln*m3h_fbw),m3h_fbw-offset);
			memsetw(m3h_mem+(ln*m3h_fbw),0x0F00,offset);
		}
	}
	else
	{
		offset *= -1;
		for ( ln=0; ln<m3h_fbh; ln++ )
		{
			memmovew(m3h_mem+(ln*m3h_fbw),m3h_mem+(ln*m3h_fbw)+offset,m3h_fbw-offset);
			memsetw(m3h_mem+(ln*m3h_fbw)+(m3h_fbw-offset),0x0F00,offset);
		}
	}
}

static void m3h_vscroll( Sint32 offset )
{
	/* noscroll */
	if ( !offset )
		return;
	/* scrolling m3h_fbh blocks? that's just like clearing the screen */
	if ( abs(offset) >= m3h_fbh )
	{
		memsetw(m3h_mem,0,2000);
		return;
	}
	/* vertical scrolling seems faster in theory */
	if ( offset > 0 )
	{
		memmovew(m3h_mem+(offset*m3h_fbw),m3h_mem,m3h_fbw*(m3h_fbh-offset));
		memsetw(m3h_mem,0x0F00,offset*m3h_fbw);
	}
	else
	{
		offset *= -1;
		memmovew(m3h_mem,m3h_mem+(offset*m3h_fbw),m3h_fbw*(m3h_fbh-offset));
		memsetw(m3h_mem+((m3h_fbh-offset)*m3h_fbw),0x0F00,offset*m3h_fbw);
	}
}

static void m3h_putpixel( Uint16 x, Uint16 y, Uint8 c )
{
	/* bloxel, actually */
	x %= mode_3h.w;
	y %= mode_3h.h;
	Uint16 *pair = m3h_mem+x+(y/2)*mode_3h.w;
	if ( c > 0x0F )
		return;
	if ( ((*pair)&0x00FF) != 0x00DF )
		*pair = 0x00DF;
	if ( y%2 )
	{
		*pair &= 0x0F00;
		*pair |= (Uint16)c<<12;
	}
	else
	{
		*pair &= 0xF000;
		*pair |= (Uint16)c<<8;
	}
	*pair |= 0x00DF;
}

static Uint8 m3h_getpixel( Uint16 x, Uint16 y )
{
	/* bloxel, actually */
	x %= mode_3h.w;
	y %= mode_3h.h;
	Uint16 *pair = m3h_mem+x+(y/2)*mode_3h.w;
	if ( y%2 )
		return ((*pair)&0xF000)>>12;
	else
		return ((*pair)&0x0F00)>>8;
}

static void m3h_drawrect( Uint16 x, Uint16 y, Uint16 w, Uint16 h, Uint8 c )
{
	Uint16 px, py;
	Uint16 lx, ly;
	px = x;
	py = y;
	lx = x+w;
	ly = y+h;
	while ( (px < lx) && (py < ly) )
	{
		m3h_putpixel(px++,py,c);
		if ( px >= lx )
		{
			px = x;
			py++;
		}
	}
}

static void m3h_drawhline( Uint16 x, Uint16 y, Uint16 l, Uint8 c )
{
	Uint16 px;
	Uint16 lx;
	px = x;
	lx = x+l;
	while ( px < lx )
		m3h_putpixel(px++,y,c);
}

static void m3h_drawvline( Uint16 x, Uint16 y, Uint16 l, Uint8 c )
{
	Uint16 py;
	Uint16 ly;
	py = y;
	ly = y+l;
	while ( py < ly )
		m3h_putpixel(x,py++,c);
}

static void m3h_drawimg( img_t *img, Uint16 x, Uint16 y, Uint16 ox, Uint16 oy, Uint16 w, Uint16 h, Uint16 palshift )
{
	if ( img->depth != IMG_DP_16COL )
	{
		/* not supported */
		m3h_drawrect(x,y,w,h,1);
		return;
	}
	Uint16 px, py;
	Uint16 lx, ly;
	Uint16 iw, ih;
	Uint16 ix, iy;
	Uint8 c;
	iw = img->w;
	ih = img->h;
	px = x;
	py = y;
	lx = x+w;
	ly = y+h;
	ix = ox;
	iy = oy;
	while ( (px < lx) && (py < ly) )
	{
		c = img->data[ix+iy*iw];
		if ( c <= 15 )
		{
			c += palshift;
			m3h_putpixel(px,py,c);
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

static void m3h_drawchar( Uint16 x, Uint16 y, char c )
{
	Uint16 px, py;
	px = x;
	py = y;
	Uint8 att[2];
	att[0] = m3h_attrs[0];
	att[1] = m3h_attrs[1];
	if ( m3h_attrs[2]&EXATTR_REVBG )
		att[1] = 15-((*(m3h_mem+px+(py*m3h_fbw))>>12)&0x0F);
	else if ( m3h_attrs[2]&EXATTR_MASKED && !att[1] )
		att[1] = ((*(m3h_mem+px+(py*m3h_fbw))>>12)&0x0F);
	if ( m3h_attrs[2]&EXATTR_NODW )
		c = m3h_mem[px+(py*m3h_fbw)]&0x00FF;
	att[0] &= 0x0F;
	att[1] &= 0x0F;
	m3h_mem[px+(py*m3h_fbw)] = (att[1]<<12)|(att[0]<<8)|c;
}

static void m3h_drawwchar( Uint16 x, Uint16 y, wchar c )
{
	return;	/* not supported */
}

static void m3h_drawstring( Uint16 x, Uint16 y, char *s )
{
	while ( *s )
		m3h_drawchar((x++),y,*(s++));
}

static void m3h_drawwstring( Uint16 x, Uint16 y, wchar *s )
{
	return;	/* not supported */
}

static void m3h_fbgetres( Uint16 *cols, Uint16 *rows )
{
	*cols = m3h_fbw;
	*rows = m3h_fbh;
}

static void m3h_fbgetcursor( Sint32 *col, Sint32 *row )
{
	*col = m3h_cx;
	*row = m3h_cy;
}

static void m3h_fbsetcursor( Sint32 col, Sint32 row )
{
	while ( col >= m3h_fbw )
		col -= m3h_fbw;
	while ( col < 0 )
		col += m3h_fbw;
	while ( row >= m3h_fbh )
		row -= m3h_fbh;
	while ( row < 0 )
		row += m3h_fbh;
	m3h_cx = col;
	m3h_cy = row;
	Uint16 cpos = col+(row*m3h_fbw);
	setvgareg(VGA_CRTC,VGA_CRTC_CSRLCH,cpos>>8);
	setvgareg(VGA_CRTC,VGA_CRTC_CSRLCL,cpos);
}

static void m3h_fbmovecursor( Sint32 cols, Sint32 rows )
{
	Sint32 px, py;
	px = m3h_cx+cols;
	py = m3h_cy+rows;
	while ( px >= m3h_fbw )
		px -= m3h_fbw;
	while ( px < 0 )
		px += m3h_fbw;
	while ( py >= m3h_fbh )
		py -= m3h_fbh;
	while ( py < 0 )
		py += m3h_fbh;
	m3h_cx = px;
	m3h_cy = py;
	Uint16 cpos = m3h_cx+(m3h_cy*m3h_fbw);
	setvgareg(VGA_CRTC,VGA_CRTC_CSRLCH,cpos>>8);
	setvgareg(VGA_CRTC,VGA_CRTC_CSRLCL,cpos);
}

static void m3h_fbcursorvis( Uint8 on )
{
	m3h_cv = on&1;
	Uint8 reg;
	if ( m3h_cv )
	{
		reg = getvgareg(VGA_CRTC,VGA_CRTC_CSRSTR)&~(1<<5);
		setvgareg(VGA_CRTC,VGA_CRTC_CSRSTR,reg);
	}
	else
	{
		reg = getvgareg(VGA_CRTC,VGA_CRTC_CSRSTR)|(1<<5);
		setvgareg(VGA_CRTC,VGA_CRTC_CSRSTR,reg);
	}
}

static void m3h_fbputc( char c )
{
	if ( m3h_cy >= m3h_fbh )	/* offscreen */
		return;
	if ( c == '\b' )
		m3h_cx -= (m3h_cx!=0)?1:0;
	else if ( c == '\t' )
		m3h_cx = (m3h_cx+8)&~(8-1);
	else if ( c == '\r' )
		m3h_cx = 0;
	else if ( c == '\n' )
	{
		m3h_cx = 0;
		m3h_cy++;
	}
	else
	{
		m3h_drawchar(m3h_cx,m3h_cy,c);
		m3h_cx++;
	}
	if ( m3h_cx >= m3h_fbw )
	{
		m3h_cx = 0;
		m3h_cy++;
	}
	if ( (m3h_cy >= m3h_fbh) && !(m3h_attrs[2]&EXATTR_NOSCR) )
	{
		m3h_vscroll(-1);
		m3h_cy--;
	}
	Uint16 cpos = m3h_cx+(m3h_cy*m3h_fbw);
	setvgareg(VGA_CRTC,VGA_CRTC_CSRLCH,cpos>>8);
	setvgareg(VGA_CRTC,VGA_CRTC_CSRLCL,cpos);
}

static void m3h_fbwputc( wchar c )
{
	return;	/* not supported */
}

static void m3h_fbputs( char *s )
{
	while ( *s )
		m3h_fbputc(*(s++));
}

static void m3h_fbwputs( wchar *s )
{
	return;	/* not supported */
}

static Uint32 m3h_vafbprintf_sattr( char *s, Uint8 ofg, Uint8 obg, Uint8 oex )
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
	m3h_fbsetattr(col&0x0F,(col>>4)&0x0F,nex);
	return (Uint32)s-(Uint32)os;
}


static Uint32 m3h_vafbprintf_curmv( char *s )
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
	m3h_fbmovecursor(x,y);
	return (Uint32)s-(Uint32)os;
}

static Uint32 m3h_vafbprintf_curset( char *s, Sint32 y )
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
	m3h_fbsetcursor(x,y);
	return (Uint32)s-(Uint32)os;
}

static void m3h_fbputu( Uint32 val, Uint16 width, Uint8 zeropad )
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
			m3h_fbputc(c[--i]);
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
			m3h_fbputc(c[--i]);
	}
}

static void m3h_fbputd( Sint32 val, Uint16 width, Uint8 zeropad )
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
			m3h_fbputc('-');
		while ( i > 0 )
			m3h_fbputc(c[--i]);
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
			m3h_fbputc('-');
		while ( i < width )
			c[i++] = (zeropad)?'0':' ';
		while ( i > 0 )
			m3h_fbputc(c[--i]);
	}
}

static void m3h_fbputh( Uint32 val, Uint16 width, Uint8 zeropad )
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
			m3h_fbputc(c[--i]);
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
			m3h_fbputc(c[--i]);
	}
}

static void m3h_fbputo( Uint32 val, Uint16 width, Uint8 zeropad )
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
			m3h_fbputc(c[--i]);
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
			m3h_fbputc(c[--i]);
	}
}

static void m3h_vafbprintf( char *s, va_list args )
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
		m3h_fbgetattr(&fg,&bg,&ex);
		m3h_fbgetcursor(&x,&y);
		alt = 0;
		zp = 0;
		wide = 0;
		if ( *s != '%' )	/* not an escape, print normally */
		{
			m3h_fbputc(*(s++));
			continue;
		}
		s++;
		if ( *s == '%' )	/* just a percent sign */
		{
			m3h_fbputc(*(s++));
			continue;
		}
		if ( *s == 's' )	/* string */
		{
			m3h_fbputs((char*)va_arg(args,char*));
			s++;
			continue;
		}
		if ( *s == 'S' )	/* wstring */
		{
			m3h_fbwputs((wchar*)va_arg(args,unsigned long*));
			s++;
			continue;
		}
		if ( *s == 'c' )	/* char */
		{
			m3h_fbputc((char)va_arg(args,int));
			s++;
			continue;
		}
		if ( *s == 'C' )	/* wchar */
		{
			m3h_fbwputc((wchar)va_arg(args,unsigned long));
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
			Uint32 skip = m3h_vafbprintf_sattr(++s,fg,bg,ex);
			s += skip;
			continue;
		}
		if ( *s == '(' )	/* cursor move */
		{
			Uint32 skip = m3h_vafbprintf_curmv(++s);
			s += skip;
			continue;
		}
		if ( *s == '{' )	/* cursor set */
		{
			Uint32 skip = m3h_vafbprintf_curset(++s,y);
			s += skip;
			continue;
		}
		while ( (*s >= '0') && (*s <= '9') )	/* field width */
			wide = wide*10+(*(s++)-'0');
		if ( *s == 'd' )	/* signed base 10 integer */
		{
			m3h_fbputd((signed long)va_arg(args,signed long),wide,zp);
			s++;
			continue;
		}
		if ( *s == 'u' )	/* unsigned base 10 integer */
		{
			m3h_fbputu((unsigned long)va_arg(args,unsigned long),wide,zp);
			s++;
			continue;
		}
		if ( *s == 'x' )	/* unsigned base 16 integer */
		{
			if ( alt )
				m3h_fbputs("0x");
			m3h_fbputh((unsigned long)va_arg(args,unsigned long),wide,zp);
			s++;
			continue;
		}
		if ( *s == 'o' )	/* unsigned base 8 integer */
		{
			if ( alt )
				m3h_fbputc('0');
			m3h_fbputo((unsigned long)va_arg(args,unsigned long),wide,zp);
			s++;
			continue;
		}
		/* meh */
		m3h_fbputc(*(s++));
	}
}

static void m3h_fbprintf( char *s, ... )
{
	va_list args;
	va_start(args,s);
	m3h_vafbprintf(s,args);
	va_end(args);
}

static void m3h_fbwprintf( wchar *s, ... )
{
	return;	/* not supported */
}

static void m3h_fbsetattr( Uint8 fg, Uint8 bg, Uint8 ex )
{
	m3h_attrs[0] = fg;
	m3h_attrs[1] = bg;
	m3h_attrs[2] = ex;
}

static void m3h_fbgetattr( Uint8 *fg, Uint8 *bg, Uint8 *ex )
{
	*fg = m3h_attrs[0];
	*bg = m3h_attrs[1];
	*ex = m3h_attrs[2];
}
