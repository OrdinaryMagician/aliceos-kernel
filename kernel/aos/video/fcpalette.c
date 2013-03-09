/*
	fcpalette.c : 16-color console palette in 32bpp.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <video/fcpalette.h>
color_t alicepalfc[16] =
{
	/* low brightness */
	{255,  0,  0,  0},
	{255,128,  0,  0},
	{255, 32,128,  0},
	{255,160, 64, 32},
	{255,  0, 32, 88},
	{255, 60,  0, 88},
	{255, 16,160,208},
	{255, 88, 88, 88},
	/* high brightness */
	{255, 32, 32, 32},
	{255,255, 64, 64},
	{255, 72,255, 64},
	{255,255,224, 60},
	{255, 48,128,255},
	{255,192, 48,255},
	{255, 72,224,255},
	{255,255,255,255},
};
