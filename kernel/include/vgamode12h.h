/*
	vgamode12h.c : Mode 12h (planar 640x480 16 colors).
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <vgamode12h.h>
#include <vgareg.h>
#include <vgamisc.h>
#include <memops.h>
#include <vgaloadimg.h>

/* mode 12h variables */
Uint8 *m12h_mem = (Uint8*)0xA0000; /* memory area */
Uint8 *m12h_fnt = NULL; /* font (currently empty) */
Uint16 m12h_cx = 0, m12h_cy = 0; /* cursor position for text */
Uint8 m12h_cv = 1; /* cursor visibility for text */
Uint16 m12h_fbw = 80, m12h_fbh = 60; /* framebuffer console columns and rows */
Uint8 m12h_attrs[3] = {7,0,0}; /* current text attributes */

/* mode 12h function prototypes */
void m12h_setmode( void );
void m12h_setpal( Uint8* pal );
void m12h_getpal( Uint8* pal );
void m12h_setfont( Uint8* fnt );
Uint8* m12h_getfont( void );
void m12h_clear( void );
void m12h_hscroll( Sint32 offset );
void m12h_vscroll( Sint32 offset );
void m12h_putpixel( Uint16 x, Uint16 y, Uint8 c );
Uint8 m12h_getpixel( Uint16 x, Uint16 y );
void m12h_drawrect( Uint16 x, Uint16 y, Uint16 w, Uint16 h, Uint8 c );
void m12h_drawhline( Uint16 x, Uint16 y, Uint16 l, Uint8 c );
void m12h_drawvline( Uint16 x, Uint16 y, Uint16 l, Uint8 c );
void m12h_drawimg( img_t *img, Uint16 x, Uint16 y, Uint16 ox, Uint16 oy, Uint16 w, Uint16 h, Uint16 palshift );
void m12h_drawchar( Uint16 x, Uint16 y, char c );
void m12h_drawwchar( Uint16 x, Uint16 y, wchar c );
void m12h_drawstring( Uint16 x, Uint16 y, char *s );
void m12h_drawwstring( Uint16 x, Uint16 y, wchar *s );
void m12h_fbgetres( Uint16 *cols, Uint16 *rows );
void m12h_fbgetcursor( Uint16 *col, Uint16 *row );
void m12h_fbsetcursor( Uint16 col, Uint16 row );
void m12h_fbmovecursor( Uint16 cols, Uint16 rows );
void m12h_fbcursorvis( Uint8 on );
void m12h_fbputc( char c );
void m12h_fbwputc( wchar c );
void m12h_fbputs( char *s );
void m12h_fbwputs( wchar *s );
void m12h_fbprintf( char *s, ... );
void m12h_fbwprintf( wchar *s, ... );
void m12h_fbsetattr( Uint8 fg, Uint8 bg, Uint8 ex );
void m12h_fbgetattr( Uint8 *fg, Uint8 *bg, Uint8 *ex );

/* mode 13h struct */
vga_mode_t mode_13h = 
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

void m12h_setmode( void )
{
	return;	/* not yet implemented */
}

void m12h_setpal( Uint8* pal )
{
	return;	/* not yet implemented */
}

void m12h_getpal( Uint8* pal )
{
	return;	/* not yet implemented */
}

void m12h_setfont( Uint8* fnt )
{
	return;	/* not yet implemented */
}

Uint8* m12h_getfont( void )
{
	return NULL;	/* not yet implemented */
}

void m12h_clear( void )
{
	return;	/* not yet implemented */
}

void m12h_hscroll( Sint32 offset )
{
	return;	/* not yet implemented */
}

void m12h_vscroll( Sint32 offset )
{
	return;	/* not yet implemented */
}

void m12h_putpixel( Uint16 x, Uint16 y, Uint8 c )
{
	return;	/* not yet implemented */
}

Uint8 m12h_getpixel( Uint16 x, Uint16 y )
{
	return 0;	/* not yet implemented */
}

void m12h_drawrect( Uint16 x, Uint16 y, Uint16 w, Uint16 h, Uint8 c )
{
	return;	/* not yet implemented */
}

void m12h_drawhline( Uint16 x, Uint16 y, Uint16 l, Uint8 c )
{
	return;	/* not yet implemented */
}

void m12h_drawvline( Uint16 x, Uint16 y, Uint16 l, Uint8 c )
{
	return;	/* not yet implemented */
}

void m12h_drawimg( img_t *img, Uint16 x, Uint16 y, Uint16 ox, Uint16 oy, Uint16 w, Uint16 h, Uint16 palshift )
{
	return;	/* not yet implemented */
}

void m12h_drawchar( Uint16 x, Uint16 y, char c )
{
	return;	/* not yet implemented */
}

void m12h_drawwchar( Uint16 x, Uint16 y, wchar c )
{
	return;	/* not yet implemented */
}

void m12h_drawstring( Uint16 x, Uint16 y, char *s )
{
	return;	/* not yet implemented */
}

void m12h_drawwstring( Uint16 x, Uint16 y, wchar *s )
{
	return;	/* not yet implemented */
}

void m12h_fbgetres( Uint16 *cols, Uint16 *rows )
{
	return;	/* not yet implemented */
}

void m12h_fbgetcursor( Uint16 *col, Uint16 *row )
{
	return;	/* not yet implemented */
}

void m12h_fbsetcursor( Uint16 col, Uint16 row )
{
	return;	/* not yet implemented */
}

void m12h_fbmovecursor( Uint16 cols, Uint16 rows )
{
	return;	/* not yet implemented */
}

void m12h_fbcursorvis( Uint8 on )
{
	return;	/* not yet implemented */
}

void m12h_fbputc( char c )
{
	return;	/* not yet implemented */
}

void m12h_fbwputc( wchar c )
{
	return;	/* not yet implemented */
}

void m12h_fbputs( char *s )
{
	return;	/* not yet implemented */
}

void m12h_fbwputs( wchar *s )
{
	return;	/* not yet implemented */
}

void m12h_fbprintf( char *s, ... )
{
	return;	/* not yet implemented */
}

void m12h_fbwprintf( wchar *s, ... )
{
	return;	/* not yet implemented */
}

void m12h_fbsetattr( Uint8 fg, Uint8 bg, Uint8 ex )
{
	return;	/* not yet implemented */
}

void m12h_fbgetattr( Uint8 *fg, Uint8 *bg, Uint8 *ex )
{
	return;	/* not yet implemented */
}
