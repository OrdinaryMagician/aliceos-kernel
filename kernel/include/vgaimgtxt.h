/*
	vgaimgtxt.h : Block graphics mode (80x50).
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef VGAIMGTXT_H
#define VGAIMGTXT_H
#include <types.h>
#include <vgatext.h>
typedef struct
{
	Uint16 w, h;
	Uint8 *data;
} bitmap_t;

/* draw image at a specific position */
void drawimg( bitmap_t *img, Uint16 posx, Uint16 posy );
/* get the color of a specific bloxel */
Uint8 vga_getbloxel( Uint16 posx, Uint16 posy );
/* set the color of a specific bloxel */
void vga_putbloxel( Uint16 posx, Uint16 posy, Uint8 color );
/* initiate block graphics mode */
void vga_initimgtxt( void );
/* return to normal text mode */
void vga_exitimgtxt( void );
#endif
