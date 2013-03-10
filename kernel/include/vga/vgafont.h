/*
	vgafont.h : BIOS font functions and patching.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef VGAFONT_H
#define VGAFONT_H
#include <sys/types.h>
/* local copy of bios font */
uint8_t biosfnt[4096];
/* apply patches to the BIOS font, save a local copy */
void initfont( void );
/* change the system font using a FBFONT formatted font as reference */
void setfont_256( uint8_t *font );
/* get the current font converting from the BIOS format to FBFONT format */
void getfont_256( uint8_t *font );
/* change the font */
void setfont( uint8_t *font );
/* get the current font */
void getfont( uint8_t *font );
/* change a specific glyph in the character map */
void setglyph( uint8_t val, uint8_t *data );
/* retrieve a specific glyph in the character map */
void getglyph( uint8_t val, uint8_t *data );
#endif
