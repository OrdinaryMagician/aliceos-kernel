/*
	loadimg.c : Image loading.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <video/loadimg.h>
#include <fs/ramdisk.h>
#include <memops.h>
/* img file magic (OS magic + file format magic) */
static Uint8 img_magic[24] =
{
	'A', 'L', 'I', 'C', 'E', 'O', 'S',
	0xE3, 0x81, 0xA7, 0xE3, 0x82, 0x8C, /* Hiragana "dere", UTF-8 */
	'R', 'A', 'W', 'I', 'M', 'G', 'F', 'I', 'L', 'E',
	'\0',
};
/* img file header structure */
typedef struct
{
	Uint8 magic[24];
	Uint8 flags;
	Uint8 depth;
	Uint16 width;
	Uint16 height;
	Uint16 palsz;
} img_header_t;
/* header is followed by palette data (if available), then image data */
/* load an image from the ramdisk */
Uint8 loadimg( img_t *dest, char *fname )
{
	/* there are many error checks here... */
	if ( !dest )
		return 1;
	rd_entry_t *img = rd_find(fname);
	if ( !img )
		return 1;
	if ( img->size < sizeof(img_header_t) )
		return 1;
	Uint8 *imgdata = (Uint8*)rd_find_data(fname);
	/* verify header magic */
	img_header_t *hd = (img_header_t*)imgdata;
	if ( memcmp(&(hd->magic[0]),&img_magic[0],24) )
		return 1;
	/* data retrieval */
	imgdata += sizeof(img_header_t);
	if ( !(hd->flags&IMG_FL_NOPAL) )
	{
		dest->pal = imgdata;
		imgdata += hd->palsz;
	}
	dest->data = imgdata;
	dest->w = hd->width;
	dest->h = hd->height;
	dest->flags = hd->flags;
	dest->depth = hd->depth;
	return 0;
}
