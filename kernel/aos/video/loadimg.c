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
static uint8_t img_magic[24] =
{
	'A', 'L', 'I', 'C', 'E', 'O', 'S',
	0xE3, 0x81, 0xA7, 0xE3, 0x82, 0x8C, /* Hiragana "dere", UTF-8 */
	'R', 'A', 'W', 'I', 'M', 'G', 'F', 'I', 'L', 'E',
	'\0',
};
/* img file header structure */
typedef struct
{
	uint8_t magic[24];
	uint8_t flags;
	uint8_t depth;
	uint16_t width;
	uint16_t height;
	uint16_t palsz;
} img_header_t;
/* header is followed by palette data (if available), then image data */
/* load an image from the ramdisk */
uint8_t loadimg( img_t *dest, char *fname )
{
	/* there are many error checks here... */
	if ( !dest )
		return 1;
	rd_entry_t *img = rd_find(fname);
	if ( !img )
		return 1;
	if ( img->size < sizeof(img_header_t) )
		return 1;
	uint8_t *imgdata = (uint8_t*)rd_find_data(fname);
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
