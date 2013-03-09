/*
	demos.c : functions to show off kernel features :P.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef DEMOS_H
#define DEMOS_H
#define DEMO_COUNT 6
typedef void (*demo_func_t)( void );
typedef struct
{
	char *name;
	char *desc;
	demo_func_t func;
} demo_t;
/* Dynamic memory demo */
void demo_kdmem( void );
/* Character map */
void demo_cmap( void );
/* list available demos */
void listdemos( void );
/* 80x50 graphics demo */
void demo_blockgfx( void );
/* Bochs/QEMU BGA demo */
void demo_bochsgfx( void );
/* mode 13h graphics demo */
void demo_realgfx( void );
/* timer demo */
void demo_timers( void );
demo_t demos[DEMO_COUNT];
#endif
