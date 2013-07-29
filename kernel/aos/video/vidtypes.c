/*
	vidtypes.c : Definitions for certain video data types.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/types.h>
#include <video/vidtypes.h>
/* easy color_t creation */
color_t color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	color_t col;
	col.r = r;
	col.g = g;
	col.b = b;
	col.a = a;
	return col;
}
