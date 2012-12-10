#ifndef SCREEN_H
#define SCREEN_H
#include "types.h"
void clear_screen( void );
void clear_screen_l( void );
void clear_screen_r( void );
void clear_line( void );
void clear_line_l( void );
void clear_line_r( void );
void putc( char c );
void puts( char *c );
void putld( Sint64 d );
void putd( Sint32 d );
void putlu( Uint64 u );
void putu( Uint32 u );
void putlh( Uint64 h );
void puth( Uint32 h );
void cur_set( int x, int y );
void cur_move( int ox, int oy );
void cur_get( int *x, int *y );
void text_getcolor( Uint8 *fg, Uint8 *bg );
void text_setcolor( Uint8 fg, Uint8 bg );
void init_video( void );
/* color table */
#define BLACK	0
#define BLUE	1
#define GREEN	2
#define CYAN	3
#define RED	4
#define MAGENTA	5
#define BROWN	6
#define LGRAY	7
#define DGRAY	8
#define LBLUE	9
#define LGREEN	10
#define LCYAN	11
#define LRED	12
#define LMAGENT 13
#define LBROWN	14
#define WHITE	15
#endif
