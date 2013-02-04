/*
	demos.c : functions to show off kernel features :P.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef DEMOS_H
#define DEMOS_H

#define DEMO_COUNT 15

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

/* mode 12h image blit (per-pixel) */
void demo_mode12imgpx( void );

/* mode 12h image blit (per-channel) */
void demo_mode12imgch( void );

/* mode 12h image blit with custom palette  (per-pixel) */
void demo_mode12imgpx_c( void );

/* mode 12h image blit with custom palette  (per-channel) */
void demo_mode12imgch_c( void );

/* Mode 12h (SLOW 640x480 planar 16-color) */
void demo_mode12( void );

/* 80x50 graphics demo */
void demo_blockgfx( void );

/* mode 13h graphics demo */
void demo_realgfx( void );

/* mode 13h graphics demo (full image alice16) */
void demo_a16gfx( void );

/* mode 13h graphics demo (full image alice64) */
void demo_a64gfx( void );

/* mode 13h graphics demo (full image alice64g) */
void demo_a64ggfx( void );

/* mode 13h graphics demo (full image custom16) */
void demo_c16gfx( void );

/* mode 13h graphics demo (full image custom32) */
void demo_c32gfx( void );

/* mode 13h graphics demo (full image custom64) */
void demo_c64gfx( void );

/* mode 13h graphics demo (full image customfull) */
void demo_cfullgfx( void );

demo_t demos[DEMO_COUNT];
#endif
