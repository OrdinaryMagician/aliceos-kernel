/*
	struct.h : Struct-based operation with graphic modes.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef VGASTRUCT_H
#define VGASTRUCT_H
#include <sys/types.h>
#include <video/loadimg.h>
#include <video/loadfnt.h>
/* memory access layout for modes */
#define LAYOUT_LINEAR  0
#define LAYOUT_PLANAR  1
#define LAYOUT_TEXT    2
/* color depths for modes */
#define DEPTH_2COLOR   0
#define DEPTH_4COLOR   1
#define DEPTH_16COLOR  2
#define DEPTH_256COLOR 3
/* extended attributes for text drawing */
#define EXATTR_MASKED  1 /* Zero brightness pixels of font (and background) are
			    transparent when drawing (NOTE: Will break cursor
			    drawing) */
#define EXATTR_HFLIP   2 /* Mirror characters horizontally */
#define EXATTR_VFLIP   4 /* Mirror characters vertically */
#define EXATTR_RCW90   8 /* Rotate 90 degrees clockwise */
#define EXATTR_RCCW90 16 /* Rotate 90 degrees counter-clockwise */
#define EXATTR_NOSCR  32 /* Character printing will never cause scrolling
			    (NOTE: if cursor goes offscreen, no more characters
			    will be drawn) */
#define EXATTR_REVBG  64 /* Characters will be drawn by inverting the
			    background colors where their glyphs have a
			    brightness above zero */
#define EXATTR_NODW  128 /* Don't draw any characters, just the background */
/* struct for video mode */
typedef struct
{
	uint16_t id; /* identificative number */
	char name[256]; /* identificative name */
	uint16_t w, h; /* resolution */
	uint8_t layout; /* memory access layout (linear or planar) */
	uint8_t depth; /* number of colors */
	void *mem; /* pointer to buffer memory */
	/* mode setting */
	void (*setmode)( void );
	/* palette setting */
	void (*setpal)( uint8_t* pal );
	void (*getpal)( uint8_t* pal );
	/* font setting */
	void (*setfont)( fnt_t* fnt );
	fnt_t* (*getfont)( void );
	/* clear screen */
	void (*clear)( void );
	/* scrolling */
	void (*hscroll)( int32_t offset );
	void (*vscroll)( int32_t offset );
	/* pixel drawing functions */
	void (*putpixel)( uint16_t x, uint16_t y, uint8_t c );
	uint8_t (*getpixel)( uint16_t x, uint16_t y );
	/* primitive drawing functions (rect, line, bitmap, char, etc.) */
	void (*drawrect)( uint16_t x, uint16_t y, uint16_t w, uint16_t h,
			  uint8_t c );
	void (*drawhline)( uint16_t x, uint16_t y, uint16_t l, uint8_t c );
	void (*drawvline)( uint16_t x, uint16_t y, uint16_t l, uint8_t c );
	void (*drawimg)( img_t *img, uint16_t x, uint16_t y, uint16_t ox,
			 uint16_t oy, uint16_t w, uint16_t h,
			 uint16_t palshift );
	void (*drawchar)( uint16_t x, uint16_t y, char c );
	void (*drawwchar)( uint16_t x, uint16_t y, wchar c );
	void (*drawstring)( uint16_t x, uint16_t y, char *s );
	void (*drawwstring)( uint16_t x, uint16_t y, wchar *s );
	/* framebuffer (console) management */
	void (*fbgetres)( uint16_t *cols, uint16_t *rows );
	void (*fbgetcursor)( int32_t *col, int32_t *row );
	void (*fbsetcursor)( int32_t col, int32_t row );
	void (*fbmovecursor)( int32_t cols, int32_t rows );
	void (*fbcursorvis)( uint8_t on );
	void (*fbputc)( char c );
	void (*fbwputc)( wchar c );
	void (*fbputs)( char *s );
	void (*fbwputs)( wchar *s );
	void (*fbprintf)( char *s, ... );
	void (*fbwprintf)( wchar *s, ... );
	void (*fbsetattr)( uint8_t fg, uint8_t bg, uint8_t ex );
	void (*fbgetattr)( uint8_t *fg, uint8_t *bg, uint8_t *ex );
} vga_mode_t;
#endif
