/*
	bga.c : Bochs/QEMU BGA graphics driver.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/types.h>
#include <video/loadfnt.h>
#include <video/loadimg.h>
#include <video/vidtypes.h>
#include <sys/port.h>
#include <bga/bochsvbe.h>
#include <bga/bga.h>
#include <memops.h>
#include <sys/helpers.h>
#include <sys/va_list.h>
/* bga variables */
static uint8_t *bga_mem = NULL; /* buffer memory area */
static fnt_t bga_fnt; /* font (currently empty) */
static int32_t bga_cx = 0, bga_cy = 0; /* cursor position for text */
static uint8_t bga_cv = 1; /* cursor visibility for text */
static uint16_t bga_fbw = 0, bga_fbh = 0; /* text columns and rows */
static uint8_t bga_attrs[3] = {7,0,0}; /* current text attributes */
static color_t bga_fbpal[16];	/* 16-color palette for framebuffer text */
static uint8_t cbank = 0;
/* some necessary functions */
void setbgareg(uint8_t reg, uint16_t val)
{
	outport_w(BGA_PORT,reg);
	outport_w(BGA_PORT+1,val);
}
uint16_t getbgareg(uint8_t reg)
{
	outport_w(BGA_PORT,reg);
	return inport_w(BGA_PORT+1);
}
void bga_bankswitch(uint8_t bank)
{
	setbgareg(BGA_REG_BANK,bank);
	cbank = bank;
}
/* bga function prototypes */
static uint8_t bga_setmode( uint16_t w, uint16_t h );
static void bga_setpal( color_t* pal );
static void bga_getpal( color_t* pal );
static void bga_setfont( fnt_t* fnt );
static fnt_t* bga_getfont( void );
static void bga_clear( void );
static void bga_hscroll( int32_t offset );
static void bga_vscroll( int32_t offset );
static void bga_putpixel( uint16_t x, uint16_t y, color_t c );
static color_t bga_getpixel( uint16_t x, uint16_t y );
static void bga_drawrect( uint16_t x, uint16_t y, uint16_t w, uint16_t h,
			  color_t c );
static void bga_drawhline( uint16_t x, uint16_t y, uint16_t l, color_t c );
static void bga_drawvline( uint16_t x, uint16_t y, uint16_t l, color_t c );
static void bga_drawimg( img_t *img, uint16_t x, uint16_t y, uint16_t ox,
			 uint16_t oy, uint16_t w, uint16_t h );
static void bga_drawchar( uint16_t x, uint16_t y, char c );
static void bga_drawwchar( uint16_t x, uint16_t y, wchar c );
static void bga_drawstring( uint16_t x, uint16_t y, char *s );
static void bga_drawwstring( uint16_t x, uint16_t y, wchar *s );
static void bga_fbgetres( uint16_t *cols, uint16_t *rows );
static void bga_fbgetcursor( int32_t *col, int32_t *row );
static void bga_fbsetcursor( int32_t col, int32_t row );
static void bga_fbmovecursor( int32_t cols, int32_t rows );
static void bga_fbcursorvis( uint8_t on );
static void bga_fbputc( char c );
static void bga_fbwputc( wchar c );
static void bga_fbputs( char *s );
static void bga_fbwputs( wchar *s );
static void bga_fbprintf( char *s, ... );
static void bga_fbwprintf( wchar *s, ... );
static void bga_fbsetattr( uint8_t fg, uint8_t bg, uint8_t ex );
static void bga_fbgetattr( uint8_t *fg, uint8_t *bg, uint8_t *ex );
/* bga driver struct */
bga_driver_t bga_drv =
{
	.id = 0xDE,
	.name = "Bochs/QEMU BGA",
	.w = 0,
	.h = 0,
	.mem = NULL,
	.setmode = bga_setmode,
	.setpal = bga_setpal,
	.getpal = bga_getpal,
	.setfont = bga_setfont,
	.getfont = bga_getfont,
	.clear = bga_clear,
	.hscroll = bga_hscroll,
	.vscroll = bga_vscroll,
	.putpixel = bga_putpixel,
	.getpixel = bga_getpixel,
	.drawrect = bga_drawrect,
	.drawhline = bga_drawhline,
	.drawvline = bga_drawvline,
	.drawimg = bga_drawimg,
	.drawchar = bga_drawchar,
	.drawwchar = bga_drawwchar,
	.drawstring = bga_drawstring,
	.drawwstring = bga_drawwstring,
	.fbgetres = bga_fbgetres,
	.fbgetcursor = bga_fbgetcursor,
	.fbsetcursor = bga_fbsetcursor,
	.fbmovecursor = bga_fbmovecursor,
	.fbcursorvis = bga_fbcursorvis,
	.fbputc = bga_fbputc,
	.fbwputc = bga_fbwputc,
	.fbputs = bga_fbputs,
	.fbwputs = bga_fbwputs,
	.fbprintf = bga_fbprintf,
	.fbwprintf = bga_fbwprintf,
	.fbsetattr = bga_fbsetattr,
	.fbgetattr = bga_fbgetattr,
};
static uint8_t bga_setmode( uint16_t w, uint16_t h )
{
	/* check if we can ACTUALLY use BGA */
	uint16_t id = getbgareg(BGA_REG_VER);
	/* we should allow only minimum version 3, but QEMU always reports 0 */
	if ( (id < BGA_VERL) || (id > BGA_VERH) )
		return 1;
	/* VBE must be disabled before changing mode */
	setbgareg(BGA_REG_ENABLE,BGA_DISABLED);
	/* parameter validation */
	if ( (w > BGA_MAX_XRES) || (h > BGA_MAX_YRES) )
		return 2;
	/* actual mode setting */
	setbgareg(BGA_REG_XRES,w);
	setbgareg(BGA_REG_YRES,h);
	setbgareg(BGA_REG_BPP,BGA_BPP_32);	/* always 32BPP */
	/* no virtual resolution and offset thingies will be used */
	setbgareg(BGA_REG_VXRES,w);
	setbgareg(BGA_REG_VXRES,h);
	setbgareg(BGA_REG_XOFF,0);
	setbgareg(BGA_REG_YOFF,0);
	/* re-enable */
	setbgareg(BGA_REG_ENABLE,BGA_ENABLED);
	bga_mem = (uint8_t*)BGA_BANK_ADDR;
	bga_drv.w = w;
	bga_drv.h = h;
	bga_drv.mem = bga_mem;
	setbgareg(BGA_REG_BANK,0);
	cbank = 0;
	bga_clear();
	return 0;
}
static void bga_setpal( color_t* pal )
{
	memcpy(&bga_fbpal,pal,sizeof(color_t)*16);
}
static void bga_getpal( color_t* pal )
{
	memcpy(pal,&bga_fbpal,sizeof(color_t)*16);
}
static void bga_setfont( fnt_t* fnt )
{
	memcpy(&bga_fnt,fnt,sizeof(fnt_t));
	/* update framebuffer res */
	bga_fbw = bga_drv.w/bga_fnt.w;
	bga_fbh = bga_drv.h/bga_fnt.h;
}
static fnt_t* bga_getfont( void )
{
	return &bga_fnt;
}
static void bga_clear( void )
{
	uint8_t nbank = 0;
	while ( nbank < BGA_BANK_MAX )
	{
		if ( nbank*BGA_BANK_SIZE >= (bga_drv.w*bga_drv.h*4) )
			break;
		nbank++;
	}
	uint8_t i;
	for ( i=0; i<nbank; i++ )
	{
		bga_bankswitch(i);
		memset(bga_mem,0,BGA_BANK_SIZE);
	}
}
static void bga_linescrh( uint16_t y, int32_t o )
{
	uint16_t x;
	if ( o < 0 )
	{
		o *= -1;
		for ( x=o; x<bga_drv.w; x++ )
			bga_putpixel(x-o,y,bga_getpixel(x,y));
		for ( x=bga_drv.w-o; x<bga_drv.w; x++ )
			bga_putpixel(x,y,color(0,0,0,0));
		return;
	}
	for ( x=0; x<bga_drv.w-o; x++ )
		bga_putpixel(x,y,bga_getpixel(x+o,y));
	for ( x=o; x<bga_drv.w-o; x++ )
		bga_putpixel(x,y,color(0,0,0,0));
}
static void bga_hscroll( int32_t offset )
{
	/* noscroll */
	if ( !offset )
		return;
	/* scrolling offscreen? that's just like clearing it */
	if ( abs(offset) >= bga_drv.w )
	{
		bga_clear();
		return;
	}
	uint16_t y;
	/* due to the way I'm using banks, this will be slow */
	for ( y=0; y<bga_drv.h; y++ )
		bga_linescrh(y,offset);
}
static void bga_linescrv( uint16_t y, int32_t o )
{
	uint16_t x;
	if ( o < 0 )
	{
		o *= -1;
		for ( x=0; x<bga_drv.w; x++ )
			bga_putpixel(x,y,(y<(bga_drv.h-o))?bga_getpixel(x,y+o)
					:color(0,0,0,0));
		return;
	}
	for ( x=0; x<bga_drv.w; x++ )
		bga_putpixel(x,y,(y<o)?bga_getpixel(x,y-o):color(0,0,0,0));
}
static void bga_vscroll( int32_t offset )
{
	/* noscroll */
	if ( !offset )
		return;
	/* scrolling offscreen? that's just like clearing it */
	if ( abs(offset) >= bga_drv.h )
	{
		bga_clear();
		return;
	}
	uint16_t y;
	/* this is going to be EVEN SLOWER than horizontal scrolling */
	/* better grab some popcorn */
	for ( y=0; y<bga_drv.h; y++ )
		bga_linescrv(y,offset);
}
static void bga_putpixel( uint16_t x, uint16_t y, color_t c )
{
	/* find what bank we need to be on */
	uint32_t off = (x+y*bga_drv.w)*4;
	uint8_t bank = off/BGA_BANK_SIZE;
	off -= bank*BGA_BANK_SIZE;
	if ( bank != cbank )
		bga_bankswitch(bank);
	/* color values are in reverse order */
	bga_mem[off++] = c.b;
	bga_mem[off++] = c.g;
	bga_mem[off++] = c.r;
	bga_mem[off] = c.a;
}
static color_t bga_getpixel( uint16_t x, uint16_t y )
{
	/* find what bank we need to be on */
	uint32_t off = (x+y*bga_drv.w)*4;
	uint8_t bank = off/BGA_BANK_SIZE;
	off -= bank*BGA_BANK_SIZE;
	if ( bank != cbank )
		bga_bankswitch(bank);
	/* color values are in reverse order */
	color_t got;
	got.b = bga_mem[off++];
	got.g = bga_mem[off++];
	got.r = bga_mem[off++];
	got.a = bga_mem[off];
	return got;
}
static void bga_drawrect( uint16_t x, uint16_t y, uint16_t w, uint16_t h,
			  color_t c )
{
	uint16_t px, py;
	uint16_t lx, ly;
	px = x;
	py = y;
	lx = x+w;
	ly = y+h;
	while ( (px < lx) && (py < ly) )
	{
		bga_putpixel(px,py,c);
		if ( (++px) < lx )
			continue;
		px = x;
		py++;
	}
}
static void bga_drawhline( uint16_t x, uint16_t y, uint16_t l, color_t c )
{
	uint16_t px;
	uint16_t lx;
	px = x;
	lx = x+l;
	while ( px < lx )
		bga_putpixel(px++,y,c);
}
static void bga_drawvline( uint16_t x, uint16_t y, uint16_t l, color_t c )
{
	uint16_t py;
	uint16_t ly;
	py = y;
	ly = y+l;
	while ( py < ly )
		bga_putpixel(x,py++,c);
}
static void bga_drawimg( img_t *img, uint16_t x, uint16_t y, uint16_t ox,
			 uint16_t oy, uint16_t w, uint16_t h )
{
	uint8_t sk = 4;
	uint8_t usealpha = 1;
	uint8_t idx = 0;
	if ( (img->depth == IMG_DP_16COL) || (img->depth == IMG_DP_256COL) )
	{
		idx = 1;
		sk = 1;
		usealpha = 0;
	}
	else if ( img->depth == IMG_DP_RGB8 )
	{
		idx = 0;
		sk = 3;
		usealpha = 0;
	}
	else if ( img->depth != IMG_DP_RGBA8 )
	{
		/* not supported */
		bga_drawrect(x,y,w,h,COLOR_RED(255));
		return;
	}
	uint16_t px, py;
	uint16_t lx, ly;
	uint16_t iw, ih;
	iw = img->w*sk;
	ih = img->h;
	px = x;
	py = y;
	lx = x+w;
	ly = y+h;
	uint16_t ix, iy;
	ix = (ox*sk);
	iy = oy;
	color_t imgc;
	while ( (px < lx) && (py < ly) )
	{
		if ( idx )
		{
			imgc.r = bga_fbpal[img->data[ix+iy*iw]&15].r;
			imgc.g = bga_fbpal[img->data[ix+iy*iw]&15].g;
			imgc.b = bga_fbpal[img->data[ix+iy*iw]&15].b;
			imgc.a = bga_fbpal[img->data[ix+iy*iw]&15].a;
			if ( img->data[ix+iy*iw] == 0xFF )
				imgc = bga_getpixel(px,py);
		}
		else
		{
			imgc.r = img->data[ix+iy*iw];
			imgc.g = img->data[ix+iy*iw+1];
			imgc.b = img->data[ix+iy*iw+2];
			imgc.a = (usealpha)?img->data[ix+iy*iw+3]:0xFF;
		}
		bga_putpixel(px,py,imgc);
		ix+=sk;
		if ( ix >= (iw*sk) )
			ix = (ox*sk);
		if ( (++px) < lx )
			continue;
		ix = (ox*sk);
		px = x;
		py++;
		if ( (++iy) < ih )
			continue;
		iy = oy;
	}
}
static void bga_drawchar( uint16_t x, uint16_t y, char c )
{
	uint16_t px, py;
	uint16_t lx, ly;
	uint8_t cw, ch;
	cw = bga_fnt.w;
	ch = bga_fnt.h;
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
		color_t c2;
		uint16_t cx2, cy2;
		uint16_t ctmp;
		cx2 = cx;
		cy2 = cy;
		if ( bga_attrs[2]&EXATTR_HFLIP )
			cx2 = (cw-1)-cx;
		if ( bga_attrs[2]&EXATTR_VFLIP )
			cy2 = (ch-1)-cy;
		if ( bga_attrs[2]&EXATTR_RCW90 )
		{
			ctmp = (cw-1)-cx2;
			cx2 = cy2;
			cy2 = ctmp;
		}
		if ( bga_attrs[2]&EXATTR_RCCW90 )
		{
			ctmp = (ch-1)-cy2;
			cy2 = cx2;
			cx2 = ctmp;
		}
		color_t revcolor = bga_getpixel(px,py);
		revcolor.r = 255-revcolor.r;
		revcolor.g = 255-revcolor.g;
		revcolor.b = 255-revcolor.b;
		c2 = bga_fbpal[bga_attrs[1]];
		if ( bga_fnt.data[off+cx2+cy2*cw] )
			c2 = (bga_attrs[2]&EXATTR_REVBG)?revcolor
				:bga_fbpal[bga_attrs[0]];
		else if ( bga_attrs[2]&EXATTR_MASKED && !bga_attrs[1] )
			c2 = bga_getpixel(px,py);
		bga_putpixel(px,py,(bga_attrs[2]&EXATTR_NODW)
				?bga_fbpal[bga_attrs[1]]:c2);
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
static void bga_drawwchar( uint16_t x, uint16_t y, wchar c )
{
	(void)x, (void)y, (void)c;
	/* not yet implemented */
}
static void bga_drawstring( uint16_t x, uint16_t y, char *s )
{
	while ( *s )
	{
		bga_drawchar(x,y,*(s++));
		x += bga_fnt.w;
	}
}
static void bga_drawwstring( uint16_t x, uint16_t y, wchar *s )
{
	while ( *s )
	{
		bga_drawwchar(x,y,*(s++));
		x += bga_fnt.w;
	}
}
static void bga_fbgetres( uint16_t *cols, uint16_t *rows )
{
	*cols = bga_fbw;
	*rows = bga_fbh;
}
static void bga_fbgetcursor( int32_t *col, int32_t *row )
{
	*col = bga_cx;
	*row = bga_cy;
}
static void bga_fbsetcursor( int32_t col, int32_t row )
{
	while ( col >= bga_fbw )
		col -= bga_fbw;
	while ( col < 0 )
		col += bga_fbw;
	while ( row >= bga_fbh )
		row -= bga_fbh;
	while ( row < 0 )
		row += bga_fbh;
	bga_cx = col;
	bga_cy = row;
}
static void bga_fbmovecursor( int32_t cols, int32_t rows )
{
	int32_t px, py;
	px = bga_cx+cols;
	py = bga_cy+rows;
	while ( px >= bga_fbw )
		px -= bga_fbw;
	while ( px < 0 )
		px += bga_fbw;
	while ( py >= bga_fbh )
		py -= bga_fbh;
	while ( py < 0 )
		py += bga_fbh;
	bga_cx = px;
	bga_cy = py;
}
static void bga_fbcursorvis( uint8_t on )
{
	bga_cv = on&1;
}
static void bga_fbputc( char c )
{
	if ( bga_cy >= bga_fbh )	/* offscreen */
		return;
	if ( c == '\b' )
		bga_cx -= (bga_cx!=0)?1:0;
	else if ( c == '\t' )
		bga_cx = (bga_cx+8)&~(8-1);
	else if ( c == '\r' )
		bga_cx = 0;
	else if ( c == '\n' )
	{
		bga_cx = 0;
		bga_cy++;
	}
	else
		bga_drawchar((bga_cx++)*bga_fnt.w,bga_cy*bga_fnt.h,c);
	if ( bga_cx >= bga_fbw )
	{
		bga_cx = 0;
		bga_cy++;
	}
	if ( (bga_cy >= bga_fbh) && !(bga_attrs[2]&EXATTR_NOSCR) )
	{
		bga_vscroll(bga_fnt.h*(-1));
		bga_cy--;
	}
}
static void bga_fbwputc( wchar c )
{
	(void)c;
	/* not yet implemented */
}
static void bga_fbputs( char *s )
{
	while ( *s )
		bga_fbputc(*(s++));
}
static void bga_fbwputs( wchar *s )
{
	while ( *s )
		bga_fbwputc(*(s++));
}
static uint32_t bga_vafbprintf_sattr( char *s, uint8_t ofg, uint8_t obg,
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
	bga_fbsetattr(col&0x0F,(col>>4)&0x0F,nex);
	return (uint32_t)s-(uint32_t)os;
}
static uint32_t bga_vafbprintf_curmv( char *s )
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
	bga_fbmovecursor(x,y);
	return (uint32_t)s-(uint32_t)os;
}
static uint32_t bga_vafbprintf_curset( char *s, int32_t y )
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
	bga_fbsetcursor(x,y);
	return (uint32_t)s-(uint32_t)os;
}
static void bga_fbputu( uint32_t val, uint16_t width, uint8_t zeropad )
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
			bga_fbputc(c[--i]);
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
			bga_fbputc(c[--i]);
	}
}
static void bga_fbputd( int32_t val, uint16_t width, uint8_t zeropad )
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
			bga_fbputc('-');
		while ( i > 0 )
			bga_fbputc(c[--i]);
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
			bga_fbputc('-');
		while ( i < width )
			c[i++] = (zeropad)?'0':' ';
		while ( i > 0 )
			bga_fbputc(c[--i]);
	}
}
static void bga_fbputh( uint32_t val, uint16_t width, uint8_t zeropad )
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
			bga_fbputc(c[--i]);
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
			bga_fbputc(c[--i]);
	}
}
static void bga_fbputo( uint32_t val, uint16_t width, uint8_t zeropad )
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
			bga_fbputc(c[--i]);
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
			bga_fbputc(c[--i]);
	}
}
static void bga_vafbprintf( char *s, va_list args )
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
		bga_fbgetattr(&fg,&bg,&ex);
		bga_fbgetcursor(&x,&y);
		alt = 0;
		zp = 0;
		wide = 0;
		if ( *s != '%' ) /* not an escape, print normally */
		{
			bga_fbputc(*(s++));
			continue;
		}
		s++;
		if ( *s == '%' ) /* just a percent sign */
		{
			bga_fbputc(*(s++));
			continue;
		}
		if ( *s == 's' ) /* string */
		{
			bga_fbputs((char*)va_arg(args,char*));
			s++;
			continue;
		}
		if ( *s == 'S' ) /* wstring */
		{
			bga_fbwputs((wchar*)va_arg(args,wchar*));
			s++;
			continue;
		}
		if ( *s == 'c' ) /* char */
		{
			bga_fbputc((char)va_arg(args,int));
			s++;
			continue;
		}
		if ( *s == 'C' ) /* wchar */
		{
			bga_fbwputc((wchar)va_arg(args,wchar));
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
			uint32_t skip = bga_vafbprintf_sattr(++s,fg,bg,ex);
			s += skip;
			continue;
		}
		if ( *s == '(' ) /* cursor move */
		{
			uint32_t skip = bga_vafbprintf_curmv(++s);
			s += skip;
			continue;
		}
		if ( *s == '{' ) /* cursor set */
		{
			uint32_t skip = bga_vafbprintf_curset(++s,y);
			s += skip;
			continue;
		}
		while ( (*s >= '0') && (*s <= '9') ) /* field width */
			wide = wide*10+(*(s++)-'0');
		if ( *s == 'd' ) /* signed base 10 integer */
		{
			bga_fbputd((int32_t)va_arg(args,int32_t),wide,zp);
			s++;
			continue;
		}
		if ( *s == 'u' ) /* unsigned base 10 integer */
		{
			bga_fbputu((uint32_t)va_arg(args,uint32_t),wide,zp);
			s++;
			continue;
		}
		if ( *s == 'x' ) /* unsigned base 16 integer */
		{
			if ( alt )
				bga_fbputs("0x");
			bga_fbputh((uint32_t)va_arg(args,uint32_t),wide,zp);
			s++;
			continue;
		}
		if ( *s == 'o' ) /* unsigned base 8 integer */
		{
			if ( alt )
				bga_fbputc('0');
			bga_fbputo((uint32_t)va_arg(args,uint32_t),wide,zp);
			s++;
			continue;
		}
		/* meh */
		bga_fbputc(*(s++));
	}
}
static void bga_fbprintf( char *s, ... )
{
	va_list args;
	va_start(args,s);
	bga_vafbprintf(s,args);
	va_end(args);
}
static void bga_fbwprintf( wchar *s, ... )
{
	(void)s;
	/* not yet implemented */
}
static void bga_fbsetattr( uint8_t fg, uint8_t bg, uint8_t ex )
{
	bga_attrs[0] = fg;
	bga_attrs[1] = bg;
	bga_attrs[2] = ex;
}
static void bga_fbgetattr( uint8_t *fg, uint8_t *bg, uint8_t *ex )
{
	*fg = bga_attrs[0];
	*bg = bga_attrs[1];
	*ex = bga_attrs[2];
}
