/*
	font.h : BIOS font functions and patching.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef FONT_H
#define FONT_H
#include <sys/types.h>
/* local copy of bios font */
Uint8 biosfnt[4096];
/* apply patches to the BIOS font, save a local copy */
void initfont( void );
/* change the font using a byte-per-pixel font as reference instead of a bit-per-pixel font */
void setfont_256( Uint8 *font );
/* get the current font converting from the BIOS format to FBFONT format */
void getfont_256( Uint8 *font );
/* change the font */
void setfont( Uint8 *font );
/* get the current font */
void getfont( Uint8 *font );
/* change a specific glyph in the character map */
void setglyph( Uint8 val, Uint8 *data );
/* retrieve a specific glyph in the character map */
void getglyph( Uint8 val, Uint8 *data );
#endif
