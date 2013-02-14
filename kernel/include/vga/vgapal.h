/*
	vgapal.h : Colors!
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef PALETTES_H
#define PALETTES_H
#include <sys/types.h>
/* Alice standard 16 color palette, reduced from 8-bit to DAC's 6-bit */
Uint8 alicepal[48];
/* Alice palette color names */
#define APAL_BLACK       0
#define APAL_MAROON      1
#define APAL_GREEN       2
#define APAL_BROWN       3
#define APAL_BLUE        4
#define APAL_PURPLE      5
#define APAL_TURQUOISE   6
#define APAL_LIGHTGRAY   7
#define APAL_DARKGRAY    8
#define APAL_RED         9
#define APAL_LIGHTGREEN 10
#define APAL_YELLOW     11
#define APAL_LIGHTBLUE  12
#define APAL_MAGENTA    13
#define APAL_CYAN       14
#define APAL_WHITE      15
/* Alice standard 256 color palette, reduced from 8-bit to DAC's 6-bit */
Uint8 alicepal256[768];
/* Alice palette offsets and lengths */
#define APAL256_16PAL_POS 0
#define APAL256_16PAL_LEN 48
#define APAL256_64PAL_POS 48
#define APAL256_64PAL_LEN 192
#define APAL256_64GRY_POS 240
#define APAL256_64GRY_LEN 192
#define APAL256_16USR_POS 432
#define APAL256_16USR_LEN 48
#define APAL256_32USR_POS 480
#define APAL256_32USR_LEN 96
#define APAL256_64USR_POS 576
#define APAL256_64USR_LEN 192

#define APAL256_16PAL_OFF 0
#define APAL256_64PAL_OFF 16
#define APAL256_64GRY_OFF 80
#define APAL256_16USR_OFF 144
#define APAL256_32USR_OFF 160
#define APAL256_64USR_OFF 192

#endif
