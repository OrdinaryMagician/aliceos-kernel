/*
	vgamode13h.c : Mode 13h (linear 320x200 256 colors).
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
#include <memops.h>
/* mode 13h variables */
static uint8_t *m13h_mem = (uint8_t*)0xA0000; /* memory area */
static fnt_t m13h_fnt; /* font (currently empty) */
static int32_t m13h_cx = 0, m13h_cy = 0; /* cursor position for text */
static uint8_t m13h_cv = 1; /* cursor visibility for text */
static uint16_t m13h_fbw = 40, m13h_fbh = 25; /* text columns and rows */
static uint8_t m13h_attrs[3] = {7,0,0}; /* current text attributes */
/* mode 13h function prototypes */
static void m13h_setmode( void );
static void m13h_setpal( uint8_t* pal );
static void m13h_getpal( uint8_t* pal );
static void m13h_setfont( fnt_t* fnt );
static fnt_t* m13h_getfont( void );
static void m13h_clear( void );
static void m13h_hscroll( int32_t offset );
static void m13h_vscroll( int32_t offset );
static void m13h_putpixel( uint16_t x, uint16_t y, uint8_t c );
static uint8_t m13h_getpixel( uint16_t x, uint16_t y );
static void m13h_drawrect( uint16_t x, uint16_t y, uint16_t w, uint16_t h,
			   uint8_t c );
static void m13h_drawhline( uint16_t x, uint16_t y, uint16_t l, uint8_t c );
static void m13h_drawvline( uint16_t x, uint16_t y, uint16_t l, uint8_t c );
static void m13h_drawimg( img_t *img, uint16_t x, uint16_t y, uint16_t ox,
			 uint16_t oy, uint16_t w, uint16_t h,
			 uint16_t palshift );
static void m13h_drawchar( uint16_t x, uint16_t y, char c );
static void m13h_drawwchar( uint16_t x, uint16_t y, wchar c );
static void m13h_drawstring( uint16_t x, uint16_t y, char *s );
static void m13h_drawwstring( uint16_t x, uint16_t y, wchar *s );
static void m13h_fbgetres( uint16_t *cols, uint16_t *rows );
static void m13h_fbgetcursor( int32_t *col, int32_t *row );
static void m13h_fbsetcursor( int32_t col, int32_t row );
static void m13h_fbmovecursor( int32_t cols, int32_t rows );
static void m13h_fbcursorvis( uint8_t on );
static void m13h_fbputc( char c );
static void m13h_fbwputc( wchar c );
static void m13h_fbputs( char *s );
static void m13h_fbwputs( wchar *s );
static void m13h_fbprintf( char *s, ... );
static void m13h_fbwprintf( wchar *s, ... );
static void m13h_fbsetattr( uint8_t fg, uint8_t bg, uint8_t ex );
static void m13h_fbgetattr( uint8_t *fg, uint8_t *bg, uint8_t *ex );
/* mode 13h struct */
vga_mode_t mode_13h =
{
	.id = 0x13,
	.name = "Mode 13h",
	.w = 320,
	.h = 200,
	.layout = LAYOUT_LINEAR,
	.depth = DEPTH_256COLOR,
	.mem = (void*)0xA0000,
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
static void m13h_setmode( void )
{
	int i;
	uint8_t regs[] =
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
	uint8_t reg;
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
	memset(m13h_mem,0x00,64000);
}
static void m13h_setpal( uint8_t* pal )
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
static void m13h_getpal( uint8_t* pal )
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
static void m13h_setfont( fnt_t* fnt )
{
	m13h_fnt = *fnt;
}
static fnt_t* m13h_getfont( void )
{
	return &m13h_fnt;
}
static void m13h_clear( void )
{
	memset(m13h_mem,0x00,64000);
}
static void m13h_hscroll( int32_t offset )
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
	uint16_t ln;
	if ( offset > 0 )
	{
		for ( ln=0; ln<200; ln++ )
		{
			memmove(m13h_mem+(ln*320)+offset,m13h_mem+(ln*320),
				320-offset);
			memset(m13h_mem+(ln*320),0x00,offset);
		}
	}
	else
	{
		offset *= -1;
		for ( ln=0; ln<200; ln++ )
		{
			memmove(m13h_mem+(ln*320),m13h_mem+(ln*320)+offset,
				320-offset);
			memset(m13h_mem+(ln*320)+(320-offset),0x00,offset);
		}
	}
}
static void m13h_vscroll( int32_t offset )
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
		memmove(m13h_mem+(offset*320),m13h_mem,320*(200-offset));
		memset(m13h_mem,0x00,offset*320);
	}
	else
	{
		offset *= -1;
		memmove(m13h_mem,m13h_mem+(offset*320),320*(200-offset));
		memset(m13h_mem+((200-offset)*320),0x00,offset*320);
	}
}
static void m13h_putpixel( uint16_t x, uint16_t y, uint8_t c )
{
	x %= 320;
	y %= 200;
	m13h_mem[x+y*320] = c;
}
static uint8_t m13h_getpixel( uint16_t x, uint16_t y )
{
	x %= 320;
	y %= 200;
	return m13h_mem[x+y*320];
}
static void m13h_drawrect( uint16_t x, uint16_t y, uint16_t w, uint16_t h,
			   uint8_t c )
{
	uint16_t px, py;
	uint16_t lx, ly;
	px = x;
	py = y;
	lx = x+w;
	ly = y+h;
	while ( (px < lx) && (py < ly) )
	{
		m13h_mem[px+py*320] = c;
		if ( (++px) < lx )
			continue;
		px = x;
		py++;
	}
}
static void m13h_drawhline( uint16_t x, uint16_t y, uint16_t l, uint8_t c )
{
	uint16_t px;
	uint16_t lx;
	px = x;
	lx = x+l;
	while ( px < lx )
		m13h_mem[(px++)+y*320] = c;
}
static void m13h_drawvline( uint16_t x, uint16_t y, uint16_t l, uint8_t c )
{
	uint16_t py;
	uint16_t ly;
	py = y;
	ly = y+l;
	while ( py < ly )
		m13h_mem[x+(py++)*320] = c;
}
static void m13h_drawimg( img_t *img, uint16_t x, uint16_t y, uint16_t ox,
			 uint16_t oy, uint16_t w, uint16_t h,
			 uint16_t palshift )
{
	if ( (img->depth < IMG_DP_16COL) || (img->depth > IMG_DP_256COL) )
	{
		/* not supported */
		m13h_drawrect(x,y,w,h,1);
		return;
	}
	uint16_t px, py;
	uint16_t lx, ly;
	uint16_t iw, ih;
	iw = img->w;
	ih = img->h;
	px = x;
	py = y;
	lx = x+w;
	ly = y+h;
	uint16_t ix, iy;
	ix = ox;
	iy = oy;
	while ( (px < lx) && (py < ly) )
	{
		m13h_mem[px+py*320] = img->data[ix+iy*iw]+palshift;
		if ( (++ix) >= iw )
			ix = ox;
		if ( (++px) < lx )
			continue;
		ix = ox;
		px = x;
		py++;
		if ( (++iy) >= ih )
			iy = oy;
	}
}
static void m13h_drawchar( uint16_t x, uint16_t y, char c )
{
	uint16_t px, py;
	uint16_t lx, ly;
	uint8_t cw, ch;
	cw = m13h_fnt.w;
	ch = m13h_fnt.h;
	px = x;
	py = y;
	lx = x+cw;
	ly = y+ch;
	uint16_t off = c*cw*ch;
	uint16_t cx, cy;
	cx = 0;
	cy = 0;
	while ( (px < lx) && (py < ly) )
	{
		uint8_t c2;
		uint16_t cx2, cy2;
		uint16_t ctmp;
		cx2 = cx;
		cy2 = cy;
		if ( m13h_attrs[2]&EXATTR_HFLIP )
			cx2 = (cw-1)-cx;
		if ( m13h_attrs[2]&EXATTR_VFLIP )
			cy2 = (ch-1)-cy;
		if ( m13h_attrs[2]&EXATTR_RCW90 )
		{
			ctmp = (cw-1)-cx2;
			cx2 = cy2;
			cy2 = ctmp;
		}
		if ( m13h_attrs[2]&EXATTR_RCCW90 )
		{
			ctmp = (ch-1)-cy2;
			cy2 = cx2;
			cx2 = ctmp;
		}
		c2 = m13h_attrs[1];
		if ( m13h_fnt.data[off+cx2+cy2*cw] )
			c2 = (m13h_attrs[2]&EXATTR_REVBG)
				?(255-m13h_mem[px+py*320]):m13h_attrs[0];
		else if ( m13h_attrs[2]&EXATTR_MASKED && !m13h_attrs[1] )
			c2 = m13h_mem[px+py*320];
		m13h_mem[px+py*320] = (m13h_attrs[2]&EXATTR_NODW)
					?m13h_attrs[1]:c2;
		if ( (++cx) >= cw )
		{
			cx = 0;
			cy++;
		}
		if ( (++px) >= lx )
		{
			px = x;
			py++;
		}
	}
}
static void m13h_drawwchar( uint16_t x, uint16_t y, wchar c )
{
	return;	/* Not implemented */
}
static void m13h_drawstring( uint16_t x, uint16_t y, char *s )
{
	while ( *s )
	{
		m13h_drawchar(x,y,*(s++));
		x += m13h_fnt.w;
	}
}
static void m13h_drawwstring( uint16_t x, uint16_t y, wchar *s )
{
	while ( *s )
	{
		m13h_drawwchar(x,y,*(s++));
		x += m13h_fnt.w;
	}
}
static void m13h_fbgetres( uint16_t *cols, uint16_t *rows )
{
	*cols = m13h_fbw;
	*rows = m13h_fbh;
}
static void m13h_fbgetcursor( int32_t *col, int32_t *row )
{
	*col = m13h_cx;
	*row = m13h_cy;
}
static void m13h_fbsetcursor( int32_t col, int32_t row )
{
	while ( col >= m13h_fbw )
		col -= m13h_fbw;
	while ( col < 0 )
		col += m13h_fbw;
	while ( row >= m13h_fbh )
		row -= m13h_fbh;
	while ( row < 0 )
		row += m13h_fbh;
	m13h_cx = col;
	m13h_cy = row;
}
static void m13h_fbmovecursor( int32_t cols, int32_t rows )
{
	int32_t px, py;
	px = m13h_cx+cols;
	py = m13h_cy+rows;
	while ( px >= m13h_fbw )
		px -= m13h_fbw;
	while ( px < 0 )
		px += m13h_fbw;
	while ( py >= m13h_fbh )
		py -= m13h_fbh;
	while ( py < 0 )
		py += m13h_fbh;
	m13h_cx = px;
	m13h_cy = py;
}
static void m13h_fbcursorvis( uint8_t on )
{
	m13h_cv = on&1;
}
static void m13h_fbputc( char c )
{
	if ( m13h_cy >= m13h_fbh ) /* offscreen */
		return;
	if ( c == '\b' )
		m13h_cx -= (m13h_cx!=0)?1:0;
	else if ( c == '\t' )
		m13h_cx = (m13h_cx+8)&~(8-1);
	else if ( c == '\r' )
		m13h_cx = 0;
	else if ( c == '\n' )
	{
		m13h_cx = 0;
		m13h_cy++;
	}
	else
		m13h_drawchar((m13h_cx++)*m13h_fnt.w,m13h_cy*m13h_fnt.h,c);
	if ( m13h_cx >= m13h_fbw )
	{
		m13h_cx = 0;
		m13h_cy++;
	}
	if ( (m13h_cy >= m13h_fbh) && !(m13h_attrs[2]&EXATTR_NOSCR) )
	{
		m13h_vscroll(m13h_fnt.h*(-1));
		m13h_cy--;
	}
}
static void m13h_fbwputc( wchar c )
{
	return;	/* not yet implemented */
}
static void m13h_fbputs( char *s )
{
	while ( *s )
		m13h_fbputc(*(s++));
}
static void m13h_fbwputs( wchar *s )
{
	return;	/* not yet implemented */
}
static uint32_t m13h_vafbprintf_sattr( char *s, uint8_t ofg, uint8_t obg,
				       uint8_t oex )
{
	char *os = s;
	uint8_t col = obg;
	uint8_t nex = oex;
	uint8_t sh = 0;
	if ( ((*s >= '0') && (*s <= '9')) || ((*s >= 'A') && (*s <= 'F'))
		 || (*s == '-') )
	{
		if ( *s == '-' )
			col = ((col&0x0F)<<4)|obg;
		else
			col = ((col&0x0F)<<4)|(((*s <= '9')?(*s-'0')
				:((*s+0xA)-'A'))&0x0F);
		s++;
	}
	if ( ((*s >= '0') && (*s <= '9')) || ((*s >= 'A') && (*s <= 'F'))
		 || (*s == '-') )
	{
		if ( *s == '-' )
			col = ((col&0x0F)<<4)|ofg;
		else
			col = ((col&0x0F)<<4)|(((*s <= '9')?(*s-'0')
				:((*s+0xA)-'A'))&0x0F);
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
	m13h_fbsetattr(col&0x0F,(col>>4)&0x0F,nex);
	return (uint32_t)s-(uint32_t)os;
}
static uint32_t m13h_vafbprintf_curmv( char *s )
{
	char *os = s;
	uint8_t neg = 0;
	int32_t x = 0;
	int32_t y = 0;
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
	m13h_fbmovecursor(x,y);
	return (uint32_t)s-(uint32_t)os;
}
static uint32_t m13h_vafbprintf_curset( char *s, int32_t y )
{
	char *os = s;
	uint8_t neg = 0;
	int32_t x = 0;
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
	m13h_fbsetcursor(x,y);
	return (uint32_t)s-(uint32_t)os;
}
static void m13h_fbputu( uint32_t val, uint16_t width, uint8_t zeropad )
{
	if ( !width )
	{
		char c[10];
		int32_t i = 0;
		do
		{
			c[i++] = '0'+val%10;
			val /= 10;
		}
		while ( val != 0 );
		while ( i > 0 )
			m13h_fbputc(c[--i]);
	}
	else
	{
		char c[width];
		int32_t i = 0;
		do
		{
			c[i++] = '0'+val%10;
			val /= 10;
		}
		while ( (val != 0) && (i < width) );
		while ( i < width )
			c[i++] = (zeropad)?'0':' ';
		while ( i > 0 )
			m13h_fbputc(c[--i]);
	}
}
static void m13h_fbputd( int32_t val, uint16_t width, uint8_t zeropad )
{
	uint8_t isneg = (val<0);
	val = abs(val);
	if ( !width )
	{
		char c[10];
		int32_t i = 0;
		do
		{
			c[i++] = '0'+val%10;
			val /= 10;
		}
		while ( val != 0 );
		if ( isneg )
			m13h_fbputc('-');
		while ( i > 0 )
			m13h_fbputc(c[--i]);
	}
	else
	{
		char c[width];
		int32_t i = 0;
		do
		{
			c[i++] = '0'+val%10;
			val /= 10;
		}
		while ( (val != 0) && (i < width) );
		if ( isneg )
			m13h_fbputc('-');
		while ( i < width )
			c[i++] = (zeropad)?'0':' ';
		while ( i > 0 )
			m13h_fbputc(c[--i]);
	}
}
static void m13h_fbputh( uint32_t val, uint16_t width, uint8_t zeropad )
{
	if ( !width )
	{
		char c[8];
		int32_t i = 0;
		do
		{
			c[i++] = ((val&0x0F)>0x09)?('A'+(val&0x0F)-0x0A)
				:('0'+(val&0x0F));
			val >>= 4;
		}
		while ( val != 0 );
		while ( i > 0 )
			m13h_fbputc(c[--i]);
	}
	else
	{
		char c[width];
		int32_t i = 0;
		do
		{
			c[i++] = ((val&0x0F)>0x09)?('A'+(val&0x0F)-0x0A)
				:('0'+(val&0x0F));
			val >>= 4;
		}
		while ( (val != 0) && (i < width) );
		while ( i < width )
			c[i++] = (zeropad)?'0':' ';
		while ( i > 0 )
			m13h_fbputc(c[--i]);
	}
}
static void m13h_fbputo( uint32_t val, uint16_t width, uint8_t zeropad )
{
	if ( !width )
	{
		char c[11];
		int32_t i = 0;
		do
		{
			c[i++] = '0'+(val&0x07);
			val >>= 3;
		}
		while ( val != 0 );
		while ( i > 0 )
			m13h_fbputc(c[--i]);
	}
	else
	{
		char c[width];
		int32_t i = 0;
		do
		{
			c[i++] = '0'+(val&0x07);
			val >>= 3;
		}
		while ( (val != 0) && (i < width) );
		while ( i < width )
			c[i++] = (zeropad)?'0':' ';
		while ( i > 0 )
			m13h_fbputc(c[--i]);
	}
}
static void m13h_vafbprintf( char *s, va_list args )
{
	uint8_t fg, bg, ex;
	int32_t x, y;
	uint8_t alt;
	uint8_t zp;
	uint16_t wide;
	/* base loop */
	while ( *s )
	{
		x = 0;
		y = 0;
		m13h_fbgetattr(&fg,&bg,&ex);
		m13h_fbgetcursor(&x,&y);
		alt = 0;
		zp = 0;
		wide = 0;
		if ( *s != '%' ) /* not an escape, print normally */
		{
			m13h_fbputc(*(s++));
			continue;
		}
		s++;
		if ( *s == '%' ) /* just a percent sign */
		{
			m13h_fbputc(*(s++));
			continue;
		}
		if ( *s == 's' ) /* string */
		{
			m13h_fbputs((char*)va_arg(args,char*));
			s++;
			continue;
		}
		if ( *s == 'S' ) /* wstring */
		{
			m13h_fbwputs((wchar*)va_arg(args,wchar*));
			s++;
			continue;
		}
		if ( *s == 'c' ) /* char */
		{
			m13h_fbputc((char)va_arg(args,int));
			s++;
			continue;
		}
		if ( *s == 'C' ) /* wchar */
		{
			m13h_fbwputc((wchar)va_arg(args,wchar));
			s++;
			continue;
		}
		if ( *s == '#' ) /* alternate form */
		{
			alt = 1;
			s++;
		}
		if ( *s == '0' ) /* zero padding */
		{
			zp = 1;
			s++;
		}
		if ( *s == '[' ) /* attribute change */
		{
			uint32_t skip = m13h_vafbprintf_sattr(++s,fg,bg,ex);
			s += skip;
			continue;
		}
		if ( *s == '(' ) /* cursor move */
		{
			uint32_t skip = m13h_vafbprintf_curmv(++s);
			s += skip;
			continue;
		}
		if ( *s == '{' ) /* cursor set */
		{
			uint32_t skip = m13h_vafbprintf_curset(++s,y);
			s += skip;
			continue;
		}
		while ( (*s >= '0') && (*s <= '9') ) /* field width */
			wide = wide*10+(*(s++)-'0');
		if ( *s == 'd' ) /* signed base 10 integer */
		{
			m13h_fbputd((int32_t)va_arg(args,int32_t),wide,zp);
			s++;
			continue;
		}
		if ( *s == 'u' ) /* unsigned base 10 integer */
		{
			m13h_fbputu((uint32_t)va_arg(args,uint32_t),wide,zp);
			s++;
			continue;
		}
		if ( *s == 'x' ) /* unsigned base 16 integer */
		{
			if ( alt )
				m13h_fbputs("0x");
			m13h_fbputh((uint32_t)va_arg(args,uint32_t),wide,zp);
			s++;
			continue;
		}
		if ( *s == 'o' ) /* unsigned base 8 integer */
		{
			if ( alt )
				m13h_fbputc('0');
			m13h_fbputo((uint32_t)va_arg(args,uint32_t),wide,zp);
			s++;
			continue;
		}
		/* meh */
		m13h_fbputc(*(s++));
	}
}
static void m13h_fbprintf( char *s, ... )
{
	va_list args;
	va_start(args,s);
	m13h_vafbprintf(s,args);
	va_end(args);
}
static void m13h_fbwprintf( wchar *s, ... )
{
	return;	/* not yet implemented */
}
static void m13h_fbsetattr( uint8_t fg, uint8_t bg, uint8_t ex )
{
	m13h_attrs[0] = fg;
	m13h_attrs[1] = bg;
	m13h_attrs[2] = ex;
}
static void m13h_fbgetattr( uint8_t *fg, uint8_t *bg, uint8_t *ex )
{
	*fg = m13h_attrs[0];
	*bg = m13h_attrs[1];
	*ex = m13h_attrs[2];
}
