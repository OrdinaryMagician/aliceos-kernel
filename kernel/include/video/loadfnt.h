/*
	loadfnt.c : Font loading.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef LOADFNT_H
#define LOADFNT_H
#include <sys/types.h>
/* formats */
#define FNT_FMT_BNW1  0 /* 1bpp black and white */
#define FNT_FMT_BNW8  1 /* 8bpp black and white (black if zero, else white) */
#define FNT_FMT_GRY8  2 /* 8bpp grayscale */
#define FNT_FMT_RGB8  3 /* 24bpp RGB */
#define FNT_FMT_RGBA8 3 /* 32bpp RGBA */
/* flag definitions */
#define FNT_FL_RESV1   1
#define FNT_FL_RESV2   2
#define FNT_FL_RESV3   4
#define FNT_FL_RESV4   8
#define FNT_FL_RESV5  16
#define FNT_FL_RESV6  32
#define FNT_FL_RESV7  64
#define FNT_FL_RESV8 128
/* struct for a font */
typedef struct
{
	uint8_t *data;
	uint8_t w;
	uint8_t h;
	uint8_t flags;
	uint8_t format;
	uint32_t limit;
} fnt_t;
/* load a font from the ramdisk */
uint8_t loadfnt( fnt_t *dest, char *fname );
#endif
