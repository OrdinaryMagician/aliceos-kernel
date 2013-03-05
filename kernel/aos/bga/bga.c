/*
	bga.c : Bochs/QEMU BGA graphics driver.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/port.h>
#include <sys/helpers.h>
#include <sys/va_list.h>
#include <bga/bga.h>
#include <bga/bochsvbe.h>
#include <video/loadimg.h>
#include <video/loadfnt.h>
#include <video/vidtypes.h>
#include <memops.h>
#include <sys/helpers.h>
#include <kmem.h>

/* bga variables */
static Uint8 *bga_mem = (Uint8*)0; /* buffer memory area */
static fnt_t bga_fnt; /* font (currently empty) */
static Sint32 bga_cx = 0, bga_cy = 0; /* cursor position for text */
static Uint8 bga_cv = 1; /* cursor visibility for text */
static Uint16 bga_fbw = 0, bga_fbh = 0; /* framebuffer console columns and rows */
static Uint8 bga_attrs[3] = {7,0,0}; /* current text attributes */
static color_t bga_fbpal[16];	/* 16-color palette for framebuffer text */
static Uint8 cbank = 0;

/* some necessary functions */
void setbgareg(Uint8 reg, Uint16 val)
{
	outport_w(BGA_PORT,reg);
	outport_w(BGA_PORT+1,val);
}

Uint16 getbgareg(Uint8 reg)
{
	outport_w(BGA_PORT,reg);
	return inport_w(BGA_PORT+1);
}

void bga_bankswitch(Uint8 bank)
{
	setbgareg(BGA_REG_BANK,bank);
	cbank = bank;
}

/* bga function prototypes */
static Uint8 bga_setmode( Uint16 w, Uint16 h );
static void bga_setpal( color_t* pal );
static void bga_getpal( color_t* pal );
static void bga_setfont( fnt_t* fnt );
static fnt_t* bga_getfont( void );
static void bga_clear( void );
static void bga_flip( void );
static void bga_hscroll( Sint32 offset );
static void bga_vscroll( Sint32 offset );
static void bga_putpixel( Uint16 x, Uint16 y, color_t c );
static color_t bga_getpixel( Uint16 x, Uint16 y );
static void bga_drawrect( Uint16 x, Uint16 y, Uint16 w, Uint16 h, color_t c );
static void bga_drawhline( Uint16 x, Uint16 y, Uint16 l, color_t c );
static void bga_drawvline( Uint16 x, Uint16 y, Uint16 l, color_t c );
static void bga_drawimg( img_t *img, Uint16 x, Uint16 y, Uint16 ox, Uint16 oy, Uint16 w, Uint16 h );
static void bga_drawchar( Uint16 x, Uint16 y, char c );
static void bga_drawwchar( Uint16 x, Uint16 y, wchar c );
static void bga_drawstring( Uint16 x, Uint16 y, char *s );
static void bga_drawwstring( Uint16 x, Uint16 y, wchar *s );
static void bga_fbgetres( Uint16 *cols, Uint16 *rows );
static void bga_fbgetcursor( Sint32 *col, Sint32 *row );
static void bga_fbsetcursor( Sint32 col, Sint32 row );
static void bga_fbmovecursor( Sint32 cols, Sint32 rows );
static void bga_fbcursorvis( Uint8 on );
static void bga_fbputc( char c );
static void bga_fbwputc( wchar c );
static void bga_fbputs( char *s );
static void bga_fbwputs( wchar *s );
static void bga_fbprintf( char *s, ... );
static void bga_fbwprintf( wchar *s, ... );
static void bga_fbsetattr( Uint8 fg, Uint8 bg, Uint8 ex );
static void bga_fbgetattr( Uint8 *fg, Uint8 *bg, Uint8 *ex );

/* bga driver struct */
bga_driver_t bga_drv =
{
	.id = 0xDE,
	.name = "Bochs/QEMU BGA",
	.w = 0,
	.h = 0,
	.mem = (Uint8*)0,
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

static Uint8 bga_setmode( Uint16 w, Uint16 h )
{
	/* check if we can ACTUALLY use BGA */
	Uint16 id = getbgareg(BGA_REG_VER);
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
	bga_mem = (Uint8*)BGA_BANK_ADDR;
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
	memcpy((Uint8*)&bga_fbpal,(Uint8*)pal,sizeof(color_t)*16);
}

static void bga_getpal( color_t* pal )
{
	memcpy((Uint8*)pal,(Uint8*)&bga_fbpal,sizeof(color_t)*16);
}

static void bga_setfont( fnt_t* fnt )
{
	memcpy((Uint8*)&bga_fnt,(Uint8*)fnt,sizeof(fnt_t));
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
	Uint8 nbank = 0;
	while ( nbank < BGA_BANK_MAX )
	{
		if ( nbank*BGA_BANK_SIZE >= (bga_drv.w*bga_drv.h*4) )
			break;
		nbank++;
	}
	Uint8 i;
	for ( i=0; i<nbank; i++ )
	{
		bga_bankswitch(i);
		memset(bga_mem,0,BGA_BANK_SIZE);
	}
}

static void bga_linescrh( Uint16 y, Sint32 o )
{
	Uint16 x;
	if ( o < 0 )
	{
		o *= -1;
		for ( x=o; x<bga_drv.w; x++ )
			bga_putpixel(x-o,y,bga_getpixel(x,y));
		for ( x=bga_drv.w-o; x<bga_drv.w; x++ )
			bga_putpixel(x,y,color(0,0,0,0));
	}
	else
	{
		for ( x=0; x<bga_drv.w-o; x++ )
			bga_putpixel(x,y,bga_getpixel(x+o,y));
		for ( x=o; x<bga_drv.w-o; x++ )
			bga_putpixel(x,y,color(0,0,0,0));
	}
}

static void bga_hscroll( Sint32 offset )
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
	Uint16 y;
	/* due to the way I'm using banks, this will be slow */
	for ( y=0; y<bga_drv.h; y++ )
		bga_linescrh(y,offset);
}

static void bga_linescrv( Uint16 y, Sint32 o )
{
	Uint16 x;
	if ( o < 0 )
	{
		o *= -1;
		for ( x=0; x<bga_drv.w; x++ )
			bga_putpixel(x,y,(y<(bga_drv.h-o))?bga_getpixel(x,y+o):color(0,0,0,0));
	}
	else
	{
		for ( x=0; x<bga_drv.w; x++ )
			bga_putpixel(x,y,(y<o)?bga_getpixel(x,y-o):color(0,0,0,0));
	}
}

static void bga_vscroll( Sint32 offset )
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
	Uint16 y;
	/* this is going to be EVEN SLOWER than horizontal scrolling */
	/* better grab some popcorn */
	for ( y=0; y<bga_drv.h; y++ )
		bga_linescrv(y,offset);
}

static void bga_putpixel( Uint16 x, Uint16 y, color_t c )
{
	/* find what bank we need to be on */
	Uint32 off = (x+y*bga_drv.w)*4;
	Uint8 bank = off/BGA_BANK_SIZE;
	off -= bank*BGA_BANK_SIZE;
	if ( bank != cbank )
		bga_bankswitch(bank);
	/* color values are in reverse order */
	bga_mem[off] = c.b;
	bga_mem[off+1] = c.g;
	bga_mem[off+2] = c.r;
	bga_mem[off+3] = c.a;
}

static color_t bga_getpixel( Uint16 x, Uint16 y )
{
	/* find what bank we need to be on */
	Uint32 off = (x+y*bga_drv.w)*4;
	Uint8 bank = off/BGA_BANK_SIZE;
	off -= bank*BGA_BANK_SIZE;
	if ( bank != cbank )
		bga_bankswitch(bank);
	/* color values are in reverse order */
	color_t got;
	got.b = bga_mem[off];
	got.g = bga_mem[off+1];
	got.r = bga_mem[off+2];
	got.a = bga_mem[off+3];
	return got;
}

static void bga_drawrect( Uint16 x, Uint16 y, Uint16 w, Uint16 h, color_t c )
{
	Uint16 px, py;
	Uint16 lx, ly;
	px = x;
	py = y;
	lx = x+w;
	ly = y+h;
	while ( (px < lx) && (py < ly) )
	{
		bga_putpixel(px,py,c);
		px++;
		if ( px >= lx )
		{
			px = x;
			py++;
		}
	}
}

static void bga_drawhline( Uint16 x, Uint16 y, Uint16 l, color_t c )
{
	Uint16 px;
	Uint16 lx;
	px = x;
	lx = x+l;
	while ( px < lx )
		bga_putpixel(px++,y,c);
}

static void bga_drawvline( Uint16 x, Uint16 y, Uint16 l, color_t c )
{
	Uint16 py;
	Uint16 ly;
	py = y;
	ly = y+l;
	while ( py < ly )
		bga_putpixel(x,py++,c);
}

static void bga_drawimg( img_t *img, Uint16 x, Uint16 y, Uint16 ox, Uint16 oy, Uint16 w, Uint16 h )
{
	Uint8 sk = 4;
	Uint8 usealpha = 1;
	Uint8 idx = 0;
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
	Uint16 px, py;
	Uint16 lx, ly;
	Uint16 iw, ih;
	iw = img->w*sk;
	ih = img->h;
	px = x;
	py = y;
	lx = x+w;
	ly = y+h;
	Uint16 ix, iy;
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
			if ( img->data[ix+iy*iw] == 255 )
				imgc = bga_getpixel(px,py);
		}
		else
		{
			imgc.r = img->data[ix+iy*iw];
			imgc.g = img->data[ix+iy*iw+1];
			imgc.b = img->data[ix+iy*iw+2];
			imgc.a = (usealpha)?img->data[ix+iy*iw+3]:255;
		}
		bga_putpixel(px,py,imgc);
		ix+=sk;
		if ( ix >= (iw*sk) )
			ix = (ox*sk);
		px++;
		if ( px >= lx )
		{
			ix = (ox*sk);
			px = x;
			py++;
			iy++;
		}
		if ( iy >= ih )
			iy = oy;
	}
}

static void bga_drawchar( Uint16 x, Uint16 y, char c )
{
	Uint16 px, py;
	Uint16 lx, ly;
	Uint8 cw, ch;
	cw = bga_fnt.w;
	ch = bga_fnt.h;
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
		color_t c2;
		Uint16 cx2, cy2;
		Uint16 ctmp;
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
			c2 = (bga_attrs[2]&EXATTR_REVBG)?revcolor:bga_fbpal[bga_attrs[0]];
		else if ( bga_attrs[2]&EXATTR_MASKED && !bga_attrs[1] )
			c2 = bga_getpixel(px,py);
		bga_putpixel(px,py,(bga_attrs[2]&EXATTR_NODW)?bga_fbpal[bga_attrs[1]]:c2);
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

static void bga_drawwchar( Uint16 x, Uint16 y, wchar c )
{
	return;	/* not yet implemented */
}

static void bga_drawstring( Uint16 x, Uint16 y, char *s )
{
	while ( *s )
	{
		bga_drawchar(x,y,*(s++));
		x += bga_fnt.w;
	}
}

static void bga_drawwstring( Uint16 x, Uint16 y, wchar *s )
{
	while ( *s )
	{
		bga_drawwchar(x,y,*(s++));
		x += bga_fnt.w;
	}
}

static void bga_fbgetres( Uint16 *cols, Uint16 *rows )
{
	*cols = bga_fbw;
	*rows = bga_fbh;
}

static void bga_fbgetcursor( Sint32 *col, Sint32 *row )
{
	*col = bga_cx;
	*row = bga_cy;
}

static void bga_fbsetcursor( Sint32 col, Sint32 row )
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

static void bga_fbmovecursor( Sint32 cols, Sint32 rows )
{
	Sint32 px, py;
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
	bga_cx += px;
	bga_cy += py;
}

static void bga_fbcursorvis( Uint8 on )
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
	{
		bga_drawchar(bga_cx*bga_fnt.w,bga_cy*bga_fnt.h,c);
		bga_cx++;
	}
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
	return;	/* not yet implemented */
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

static Uint32 bga_vafbprintf_sattr( char *s, Uint8 ofg, Uint8 obg, Uint8 oex )
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
	bga_fbsetattr(col&0x0F,(col>>4)&0x0F,nex);
	return (Uint32)s-(Uint32)os;
}

static Uint32 bga_vafbprintf_curmv( char *s )
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
	bga_fbmovecursor(x,y);
	return (Uint32)s-(Uint32)os;
}

static Uint32 bga_vafbprintf_curset( char *s, Sint32 y )
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
	bga_fbsetcursor(x,y);
	return (Uint32)s-(Uint32)os;
}

static void bga_fbputu( Uint32 val, Uint16 width, Uint8 zeropad )
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
			bga_fbputc(c[--i]);
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
			bga_fbputc(c[--i]);
	}
}

static void bga_fbputd( Sint32 val, Uint16 width, Uint8 zeropad )
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
			bga_fbputc('-');
		while ( i > 0 )
			bga_fbputc(c[--i]);
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
			bga_fbputc('-');
		while ( i < width )
			c[i++] = (zeropad)?'0':' ';
		while ( i > 0 )
			bga_fbputc(c[--i]);
	}
}

static void bga_fbputh( Uint32 val, Uint16 width, Uint8 zeropad )
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
			bga_fbputc(c[--i]);
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
			bga_fbputc(c[--i]);
	}
}

static void bga_fbputo( Uint32 val, Uint16 width, Uint8 zeropad )
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
			bga_fbputc(c[--i]);
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
			bga_fbputc(c[--i]);
	}
}

static void bga_vafbprintf( char *s, va_list args )
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
		bga_fbgetattr(&fg,&bg,&ex);
		bga_fbgetcursor(&x,&y);
		alt = 0;
		zp = 0;
		wide = 0;
		if ( *s != '%' )	/* not an escape, print normally */
		{
			bga_fbputc(*(s++));
			continue;
		}
		s++;
		if ( *s == '%' )	/* just a percent sign */
		{
			bga_fbputc(*(s++));
			continue;
		}
		if ( *s == 's' )	/* string */
		{
			bga_fbputs((char*)va_arg(args,char*));
			s++;
			continue;
		}
		if ( *s == 'S' )	/* wstring */
		{
			bga_fbwputs((wchar*)va_arg(args,unsigned long*));
			s++;
			continue;
		}
		if ( *s == 'c' )	/* char */
		{
			bga_fbputc((char)va_arg(args,int));
			s++;
			continue;
		}
		if ( *s == 'C' )	/* wchar */
		{
			bga_fbwputc((wchar)va_arg(args,unsigned long));
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
			Uint32 skip = bga_vafbprintf_sattr(++s,fg,bg,ex);
			s += skip;
			continue;
		}
		if ( *s == '(' )	/* cursor move */
		{
			Uint32 skip = bga_vafbprintf_curmv(++s);
			s += skip;
			continue;
		}
		if ( *s == '{' )	/* cursor set */
		{
			Uint32 skip = bga_vafbprintf_curset(++s,y);
			s += skip;
			continue;
		}
		while ( (*s >= '0') && (*s <= '9') )	/* field width */
			wide = wide*10+(*(s++)-'0');
		if ( *s == 'd' )	/* signed base 10 integer */
		{
			bga_fbputd((signed long)va_arg(args,signed long),wide,zp);
			s++;
			continue;
		}
		if ( *s == 'u' )	/* unsigned base 10 integer */
		{
			bga_fbputu((unsigned long)va_arg(args,unsigned long),wide,zp);
			s++;
			continue;
		}
		if ( *s == 'x' )	/* unsigned base 16 integer */
		{
			if ( alt )
				bga_fbputs("0x");
			bga_fbputh((unsigned long)va_arg(args,unsigned long),wide,zp);
			s++;
			continue;
		}
		if ( *s == 'o' )	/* unsigned base 8 integer */
		{
			if ( alt )
				bga_fbputc('0');
			bga_fbputo((unsigned long)va_arg(args,unsigned long),wide,zp);
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
	return;	/* not yet implemented */
}

static void bga_fbsetattr( Uint8 fg, Uint8 bg, Uint8 ex )
{
	bga_attrs[0] = fg;
	bga_attrs[1] = bg;
	bga_attrs[2] = ex;
}

static void bga_fbgetattr( Uint8 *fg, Uint8 *bg, Uint8 *ex )
{
	*fg = bga_attrs[0];
	*bg = bga_attrs[1];
	*ex = bga_attrs[2];
}

