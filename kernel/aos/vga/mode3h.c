/*
	mode3h.c : Mode 3h (80x25 16-color text mode).
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/types.h>
#include <video/loadfnt.h>
#include <video/loadimg.h>
#include <vga/struct.h>
#include <sys/port.h>
#include <vga/vgareg.h>
#include <memops.h>
#include <vga/vgafont.h>
#include <sys/helpers.h>
#include <sys/va_list.h>
/* mode 3h variables */
static uint16_t *m3h_mem = (uint16_t*)0xB8000; /* memory area */
static fnt_t m3h_fnt; /* font (currently empty) */
static int32_t m3h_cx = 0, m3h_cy = 0; /* cursor position for text */
static uint8_t m3h_cv = 1; /* cursor visibility for text */
static uint16_t m3h_fbw = 80, m3h_fbh = 25; /* text columns and rows */
static uint8_t m3h_attrs[3] = {7,0,0}; /* current text attributes */
/* mode 3h function prototypes */
static void m3h_setmode( void );
static void m3h_setpal( uint8_t* pal );
static void m3h_getpal( uint8_t* pal );
static void m3h_setfont( fnt_t* fnt );
static fnt_t* m3h_getfont( void );
static void m3h_clear( void );
static void m3h_hscroll( int32_t offset );
static void m3h_vscroll( int32_t offset );
static void m3h_putpixel( uint16_t x, uint16_t y, uint8_t c );
static uint8_t m3h_getpixel( uint16_t x, uint16_t y );
static void m3h_drawrect( uint16_t x, uint16_t y, uint16_t w, uint16_t h,
			  uint8_t c );
static void m3h_drawhline( uint16_t x, uint16_t y, uint16_t l, uint8_t c );
static void m3h_drawvline( uint16_t x, uint16_t y, uint16_t l, uint8_t c );
static void m3h_drawimg( img_t *img, uint16_t x, uint16_t y, uint16_t ox,
			 uint16_t oy, uint16_t w, uint16_t h,
			 uint16_t palshift );
static void m3h_drawchar( uint16_t x, uint16_t y, char c );
static void m3h_drawwchar( uint16_t x, uint16_t y, wchar c );
static void m3h_drawstring( uint16_t x, uint16_t y, char *s );
static void m3h_drawwstring( uint16_t x, uint16_t y, wchar *s );
static void m3h_fbgetres( uint16_t *cols, uint16_t *rows );
static void m3h_fbgetcursor( int32_t *col, int32_t *row );
static void m3h_fbsetcursor( int32_t col, int32_t row );
static void m3h_fbmovecursor( int32_t cols, int32_t rows );
static void m3h_fbcursorvis( uint8_t on );
static void m3h_fbputc( char c );
static void m3h_fbwputc( wchar c );
static void m3h_fbputs( char *s );
static void m3h_fbwputs( wchar *s );
static void m3h_fbprintf( char *s, ... );
static void m3h_fbwprintf( wchar *s, ... );
static void m3h_fbsetattr( uint8_t fg, uint8_t bg, uint8_t ex );
static void m3h_fbgetattr( uint8_t *fg, uint8_t *bg, uint8_t *ex );
/* mode 3h struct */
vga_mode_t mode_3h =
{
	.id = 0x03,
	.name = "Mode 3h",
	.w = 80,
	.h = 50,
	.layout = LAYOUT_TEXT,
	.depth = DEPTH_16COLOR,
	.mem = (void*)0xB8000,
	.setmode = m3h_setmode,
	.setpal = m3h_setpal,
	.getpal = m3h_getpal,
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
	uint8_t regs[] =
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
	memsetw(m3h_mem,0x00,2000);
}
static void m3h_setpal( uint8_t* pal )
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
static void m3h_getpal( uint8_t* pal )
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
	memcpy(&m3h_fnt,fnt,sizeof(fnt_t));
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
static void m3h_hscroll( int32_t offset )
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
	uint16_t ln;
	if ( offset > 0 )
	{
		for ( ln=0; ln<m3h_fbh; ln++ )
		{
			memmovew(m3h_mem+(ln*m3h_fbw)+offset,
				m3h_mem+(ln*m3h_fbw),m3h_fbw-offset);
			memsetw(m3h_mem+(ln*m3h_fbw),0x0F00,offset);
		}
	}
	else
	{
		offset *= -1;
		for ( ln=0; ln<m3h_fbh; ln++ )
		{
			memmovew(m3h_mem+(ln*m3h_fbw),
				m3h_mem+(ln*m3h_fbw)+offset,m3h_fbw-offset);
			memsetw(m3h_mem+(ln*m3h_fbw)+(m3h_fbw-offset),
				0x0F00,offset);
		}
	}
}
static void m3h_vscroll( int32_t offset )
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
		memmovew(m3h_mem+(offset*m3h_fbw),m3h_mem,
			m3h_fbw*(m3h_fbh-offset));
		memsetw(m3h_mem,0x0F00,offset*m3h_fbw);
	}
	else
	{
		offset *= -1;
		memmovew(m3h_mem,m3h_mem+(offset*m3h_fbw),
			m3h_fbw*(m3h_fbh-offset));
		memsetw(m3h_mem+((m3h_fbh-offset)*m3h_fbw),0x0F00,
			offset*m3h_fbw);
	}
}
static void m3h_putpixel( uint16_t x, uint16_t y, uint8_t c )
{
	/* bloxel, actually */
	x %= mode_3h.w;
	y %= mode_3h.h;
	uint16_t *pair = m3h_mem+x+(y/2)*mode_3h.w;
	if ( c > 0x0F )
		return;
	if ( ((*pair)&0x00FF) != 0x00DF )
		*pair = 0x00DF;
	if ( y%2 )
	{
		*pair &= 0x0F00;
		*pair |= (uint16_t)c<<12;
	}
	else
	{
		*pair &= 0xF000;
		*pair |= (uint16_t)c<<8;
	}
	*pair |= 0x00DF;
}
static uint8_t m3h_getpixel( uint16_t x, uint16_t y )
{
	/* bloxel, actually */
	x %= mode_3h.w;
	y %= mode_3h.h;
	uint16_t *pair = m3h_mem+x+(y/2)*mode_3h.w;
	if ( y%2 )
		return ((*pair)&0xF000)>>12;
	else
		return ((*pair)&0x0F00)>>8;
}
static void m3h_drawrect( uint16_t x, uint16_t y, uint16_t w, uint16_t h,
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
		m3h_putpixel(px++,py,c);
		if ( px < lx )
			continue;
		px = x;
		py++;
	}
}
static void m3h_drawhline( uint16_t x, uint16_t y, uint16_t l, uint8_t c )
{
	uint16_t px;
	uint16_t lx;
	px = x;
	lx = x+l;
	while ( px < lx )
		m3h_putpixel(px++,y,c);
}
static void m3h_drawvline( uint16_t x, uint16_t y, uint16_t l, uint8_t c )
{
	uint16_t py;
	uint16_t ly;
	py = y;
	ly = y+l;
	while ( py < ly )
		m3h_putpixel(x,py++,c);
}
static void m3h_drawimg( img_t *img, uint16_t x, uint16_t y, uint16_t ox,
			 uint16_t oy, uint16_t w, uint16_t h,
			 uint16_t palshift )
{
	if ( img->depth != IMG_DP_16COL )
	{
		/* not supported */
		m3h_drawrect(x,y,w,h,1);
		return;
	}
	uint16_t px, py;
	uint16_t lx, ly;
	uint16_t iw, ih;
	uint16_t ix, iy;
	uint8_t c;
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
static void m3h_drawchar( uint16_t x, uint16_t y, char c )
{
	uint16_t px, py;
	px = x;
	py = y;
	uint8_t att[2];
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
static void m3h_drawwchar( uint16_t x, uint16_t y, wchar c )
{
	x=0;y=0;c=0;
	return;	/* not supported */
}
static void m3h_drawstring( uint16_t x, uint16_t y, char *s )
{
	while ( *s )
		m3h_drawchar((x++),y,*(s++));
}
static void m3h_drawwstring( uint16_t x, uint16_t y, wchar *s )
{
	x=0;y=0;s=0;
	return;	/* not supported */
}
static void m3h_fbgetres( uint16_t *cols, uint16_t *rows )
{
	*cols = m3h_fbw;
	*rows = m3h_fbh;
}
static void m3h_fbgetcursor( int32_t *col, int32_t *row )
{
	*col = m3h_cx;
	*row = m3h_cy;
}
static void m3h_fbsetcursor( int32_t col, int32_t row )
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
	uint16_t cpos = col+(row*m3h_fbw);
	setvgareg(VGA_CRTC,VGA_CRTC_CSRLCH,cpos>>8);
	setvgareg(VGA_CRTC,VGA_CRTC_CSRLCL,cpos);
}
static void m3h_fbmovecursor( int32_t cols, int32_t rows )
{
	int32_t px, py;
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
	uint16_t cpos = m3h_cx+(m3h_cy*m3h_fbw);
	setvgareg(VGA_CRTC,VGA_CRTC_CSRLCH,cpos>>8);
	setvgareg(VGA_CRTC,VGA_CRTC_CSRLCL,cpos);
}
static void m3h_fbcursorvis( uint8_t on )
{
	m3h_cv = on&1;
	uint8_t reg;
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
	if ( m3h_cy >= m3h_fbh ) /* offscreen */
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
		m3h_drawchar(m3h_cx++,m3h_cy,c);
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
	uint16_t cpos = m3h_cx+(m3h_cy*m3h_fbw);
	setvgareg(VGA_CRTC,VGA_CRTC_CSRLCH,cpos>>8);
	setvgareg(VGA_CRTC,VGA_CRTC_CSRLCL,cpos);
}
static void m3h_fbwputc( wchar c )
{
	c=0;
	return;	/* not supported */
}
static void m3h_fbputs( char *s )
{
	while ( *s )
		m3h_fbputc(*(s++));
}
static void m3h_fbwputs( wchar *s )
{
	s=0;
	return;	/* not supported */
}
static uint32_t m3h_vafbprintf_sattr( char *s, uint8_t ofg, uint8_t obg,
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
	m3h_fbsetattr(col&0x0F,(col>>4)&0x0F,nex);
	return (uint32_t)s-(uint32_t)os;
}
static uint32_t m3h_vafbprintf_curmv( char *s )
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
	m3h_fbmovecursor(x,y);
	return (uint32_t)s-(uint32_t)os;
}
static uint32_t m3h_vafbprintf_curset( char *s, int32_t y )
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
	m3h_fbsetcursor(x,y);
	return (uint32_t)s-(uint32_t)os;
}
static void m3h_fbputu( uint32_t val, uint16_t width, uint8_t zeropad )
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
			m3h_fbputc(c[--i]);
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
			m3h_fbputc(c[--i]);
	}
}
static void m3h_fbputd( int32_t val, uint16_t width, uint8_t zeropad )
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
			m3h_fbputc('-');
		while ( i > 0 )
			m3h_fbputc(c[--i]);
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
			m3h_fbputc('-');
		while ( i < width )
			c[i++] = (zeropad)?'0':' ';
		while ( i > 0 )
			m3h_fbputc(c[--i]);
	}
}
static void m3h_fbputh( uint32_t val, uint16_t width, uint8_t zeropad )
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
			m3h_fbputc(c[--i]);
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
			m3h_fbputc(c[--i]);
	}
}
static void m3h_fbputo( uint32_t val, uint16_t width, uint8_t zeropad )
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
			m3h_fbputc(c[--i]);
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
			m3h_fbputc(c[--i]);
	}
}
static void m3h_vafbprintf( char *s, va_list args )
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
		m3h_fbgetattr(&fg,&bg,&ex);
		m3h_fbgetcursor(&x,&y);
		alt = 0;
		zp = 0;
		wide = 0;
		if ( *s != '%' ) /* not an escape, print normally */
		{
			m3h_fbputc(*(s++));
			continue;
		}
		s++;
		if ( *s == '%' ) /* just a percent sign */
		{
			m3h_fbputc(*(s++));
			continue;
		}
		if ( *s == 's' ) /* string */
		{
			m3h_fbputs((char*)va_arg(args,char*));
			s++;
			continue;
		}
		if ( *s == 'S' ) /* wstring */
		{
			m3h_fbwputs((wchar*)va_arg(args,wchar*));
			s++;
			continue;
		}
		if ( *s == 'c' ) /* char */
		{
			m3h_fbputc((char)va_arg(args,int));
			s++;
			continue;
		}
		if ( *s == 'C' ) /* wchar */
		{
			m3h_fbwputc((wchar)va_arg(args,wchar));
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
			uint32_t skip = m3h_vafbprintf_sattr(++s,fg,bg,ex);
			s += skip;
			continue;
		}
		if ( *s == '(' ) /* cursor move */
		{
			uint32_t skip = m3h_vafbprintf_curmv(++s);
			s += skip;
			continue;
		}
		if ( *s == '{' ) /* cursor set */
		{
			uint32_t skip = m3h_vafbprintf_curset(++s,y);
			s += skip;
			continue;
		}
		while ( (*s >= '0') && (*s <= '9') ) /* field width */
			wide = wide*10+(*(s++)-'0');
		if ( *s == 'd' ) /* signed base 10 integer */
		{
			m3h_fbputd((int32_t)va_arg(args,int32_t),wide,zp);
			s++;
			continue;
		}
		if ( *s == 'u' ) /* unsigned base 10 integer */
		{
			m3h_fbputu((uint32_t)va_arg(args,uint32_t),wide,zp);
			s++;
			continue;
		}
		if ( *s == 'x' ) /* unsigned base 16 integer */
		{
			if ( alt )
				m3h_fbputs("0x");
			m3h_fbputh((uint32_t)va_arg(args,uint32_t),wide,zp);
			s++;
			continue;
		}
		if ( *s == 'o' ) /* unsigned base 8 integer */
		{
			if ( alt )
				m3h_fbputc('0');
			m3h_fbputo((uint32_t)va_arg(args,uint32_t),wide,zp);
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
	s=0;
	return;	/* not supported */
}
static void m3h_fbsetattr( uint8_t fg, uint8_t bg, uint8_t ex )
{
	m3h_attrs[0] = fg;
	m3h_attrs[1] = bg;
	m3h_attrs[2] = ex;
}
static void m3h_fbgetattr( uint8_t *fg, uint8_t *bg, uint8_t *ex )
{
	*fg = m3h_attrs[0];
	*bg = m3h_attrs[1];
	*ex = m3h_attrs[2];
}
