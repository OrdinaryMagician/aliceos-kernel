/*
	vgatext.h : Standard VGA text mode functionality.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef VGATEXT_H
#define VGATEXT_H
#include <types.h>
#include <helpers.h>
#include <port.h>
#include <memops.h>

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

/*
   scrolling: when the cursor is beyond 24 lines...
    1. move the last 24 lines one position back
    2. clear new line
    3. push cursor back to line 24
    4. ????
    5. profit!
*/
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
void vga_curset( int x, int y );

/* move cursor by offset */
void vga_curmv( int ox, int oy );

/* get current cursor position */
void vga_curget( int *x, int *y );

/* get text attributes */
void vga_getattr( Uint8 *fg, Uint8 *bg );

/* set text attributes */
void vga_setattr( Uint8 fg, Uint8 bg );

/* set character at position, no scrolling or cursor movement */
void vga_setc( Uint16 pos, char c, Uint8 attr );

/* put a single character on screen */
void vga_putc( char c );

/* put a base 10 unsigned integer on screen */
void vga_putu( Uint64 val, Uint16 width, Uint8 zeropad );

/* put a base 10 signed integer on screen */
void vga_putd( Sint64 val, Uint16 width, Uint8 zeropad );

#endif
