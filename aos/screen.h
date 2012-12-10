#ifndef SCREEN_H
#define SCREEN_H
void cls( void );
void putc( char c );
void puts( char *c );
void text_setcolor( unsigned char fg, unsigned char bg );
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
