/*
	vgatext.h : Standard VGA text mode functionality.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef VGATEXT_H
#define VGATEXT_H
#include <types.h>
#include <helpers.h>
#include <port.h>
#include <memops.h>
#include <vgareg.h>

/* colors valid on default palette */
#define BLACK       0
#define BLUE        1
#define GREEN       2
#define TURQUOISE   3
#define MAROON      4
#define PURPLE      5
#define BROWN       6
#define LIGHTGRAY   7
#define DARKGRAY    8
#define LIGHTBLUE   9
#define LIGHTGREEN 10
#define CYAN       11
#define RED        12
#define MAGENTA    13
#define YELLOW     14
#define WHITE      15

/* toggle automatic scrolling */
void vga_tscroll( Uint8 on );

/*  change the area where scrolling can take effect */
void vga_setcroparea( Uint8 start, Uint8 end );

/* scroll if needed */
void vga_scroll( void );

/* update cursor position */
void vga_updatecsr( void );

/* clear entire screen */
void vga_clr_s( void );

/* clear screen to the left of the cursor */
void vga_clr_sl( void );

/* clear screen to the right of the cursor */
void vga_clr_sr( void );

/* clear current line */
void vga_clr_l( void );

/* clear everything inside the current line to the left of the cursor */
void vga_clr_ll( void );

/* clear everything inside the current line to the right of the cursor */
void vga_clr_lr( void );

/* set cursor position directly */
void vga_curset( Sint32 x, Sint32 y );

/* move cursor by offset */
void vga_curmv( Sint32 ox, Sint32 oy );

/* get current cursor position */
void vga_curget( Sint32 *x, Sint32 *y );

/* get text attributes */
void vga_getattr( Uint8 *fg, Uint8 *bg );

/* set text attributes */
void vga_setattr( Uint8 fg, Uint8 bg );

/* set character at position, no scrolling or cursor movement */
void vga_setc( Uint16 pos, Uint8 c, Uint8 attr );

/* put a single character on screen */
void vga_putc( char c );

/* put an entire string on screen */
void vga_puts( char *s );

/* put a character many times on screen */
void vga_putmc( char c, Sint32 i );

/* put a base 10 unsigned integer on screen */
void vga_putu( Uint64 val, Uint16 width, Uint8 zeropad );

/* put a base 10 signed integer on screen */
void vga_putd( Sint64 val, Uint16 width, Uint8 zeropad );

/* put a base 16 unsigned integer on screen */
void vga_puth( Uint64 val, Uint16 width, Uint8 zeropad );

/* put a base 8 unsigned integer on screen */
void vga_puto( Uint64 val, Uint16 width, Uint8 zeropad );	/* no jokes about the function name, please */

/* set the 16-color 6-bit palettes */
void vga_setpal( Uint8 *pal );

/* get the 16-color 6-bit palettes */
void vga_getpal( Uint8 *pal );

/* switch to 8px wide characters */
void vga_set8dot();

/* switch to 8px wide characters */
void vga_set9dot();

/* change the font */
void vga_setfont( Uint8 *font );

/* get the current font */
void vga_getfont( Uint8 *font );

/* change a specific glyph in the character map */
void vga_setglyph( Uint8 val, Uint8 *data );

/* retrieve a specific glyph in the character map */
void vga_getglyph( Uint8 val, Uint8 *data );

/* hide cursor */
void vga_hidecursor( void );

/* show cursor */
void vga_showcursor( void );
#endif
