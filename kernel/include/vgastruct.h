/*
	vgastruct.h : Struct-based operation with graphic modes (should be more comfortable).
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/

#define LAYOUT_LINEAR  0
#define LAYOUT_PLANAR  1
#define DEPTH_2COLOR   0
#define DEPTH_4COLOR   1
#define DEPTH_16COLOR  2
#define DEPTH_256COLOR 3

/* struct for video mode */
typedef struct
{
	char name[256];	/* identificative name */
	Uint16 w, h;	/* resolution */
	Uint8 layout;	/* memory access layout (linear or planar) */
	Uint8 depth;	/* number of colors */
	Uint8 *mem;	/* pointer to framebuffer memory */
	Uint8 *pal;	/* pointer to current palette */
	Uint8 *fnt;	/* pointer to current font */
	/* mode setting */
	void (*setmode)( void );
	/* palette setting */
	void (*setpal)( Uint8* pal );
	Uint8* (*getpal)( void );
	/* font setting */
	void (*setfont)( Uint8* fnt );
	Uint8* (*getfont)( void );
	/* clear screen */
	void (*clear)( void );
	/* pixel drawing functions */
	void (*putpixel)( Uint16 x, Uint16 y, Uint8 c );
	Uint8 (*getpixel)( Uint16 x, Uint16 y );
	/* primitive drawing functions (rect, line, bitmap, char, etc.) */
	void (*drawrect)( Uint16 x, Uint16 y, Uint16 w, Uint16 h, Uint8 c );
	void (*drawhline)( Uint16 x, Uint16 y, Uint16 l, Uint8 c );
	void (*drawvline)( Uint16 x, Uint16 y, Uint16 l, Uint8 c );
	void (*drawimg)( Uint16 x, Uint16 y, Uint16 ox, Uint16 oy, Uint16 w, Uint16 h );
	void (*drawchar)( Uint16 x, Uint16 y, char c );
	void (*drawuchar)( Uint16 x, Uint16 y, uchar c );
	void (*drawstring)( Uint16 x, Uint16 y, char *s );
	/* framebuffer console management */
	void (*fbscroll)( void );
	void (*fbgetres)( Uint16 *cols, Uint16 *rows );
	void (*fbgetcursor)( Uint16 *col, Uint16 *row );
	void (*fbsetcursor)( Uint16 col, Uint16 row );
	void (*fbmovecursor)( Uint16 cols, Uint16 rows );
	void (*fbgetcursor)( Uint16 *col, Uint16 *row );
	void (*fbsetcursor)( Uint16 col, Uint16 row );
	void (*fbcursorvis)( Uint8 on );
	void (*fbputc)( char c );
	void (*fbputuc)( uchar c );
	void (*fbputs)( char *s );
	void (*fbprintf)( char *s, ... );

} vga_mode_t;
