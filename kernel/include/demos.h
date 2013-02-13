/*
	demos.c : functions to show off kernel features :P.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef DEMOS_H
#define DEMOS_H

#define DEMO_COUNT 4

typedef void (*demo_func_t)( void );
typedef struct
{
	char *name;
	char *desc;
	demo_func_t func;
} demo_t;

/* Character map */
void demo_cmap( void );

/* list available demos */
void listdemos( void );

/* 80x50 graphics demo */
void demo_blockgfx( void );

/* mode 12h graphics demo */
void demo_crapgfx( void );

/* mode 13h graphics demo */
void demo_realgfx( void );

demo_t demos[DEMO_COUNT];
#endif
