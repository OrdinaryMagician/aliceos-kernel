/*
	loadimg.h : Image loading.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef LOADIMG_H
#define LOADIMG_H
#include <sys/types.h>
/* depth types */
#define IMG_DP_MONO   0 /* 8bpp monochrome */
#define IMG_DP_4COL   1 /* 8bpp 4 color */
#define IMG_DP_16COL  2 /* 8bpp 16 color */
#define IMG_DP_256COL 3 /* 8bpp 256 color */
#define IMG_DP_RGB8   4 /* 24bpp RGB */
#define IMG_DP_RGBA8  5 /* 32bpp RGBA */

/* flag definitions */
#define IMG_FL_NOPAL   1 /* no embedded palette */
#define IMG_FL_RESV2   2
#define IMG_FL_RESV3   4
#define IMG_FL_RESV4   8
#define IMG_FL_RESV5  16
#define IMG_FL_RESV6  32
#define IMG_FL_RESV7  64
#define IMG_FL_RESV8 128

/* struct for an image */
typedef struct
{
	Uint8 *data;
	Uint8 *pal;
	Uint16 w;
	Uint16 h;
	Uint8 flags;
	Uint8 depth;
} img_t;

/* load an image from the ramdisk */
Uint8 loadimg( img_t *dest, char *fname );
#endif
