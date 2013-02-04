/*
	vgaloadfnt.c : Font loading.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <vgaloadfnt.h>
#include <ramdisk.h>
#include <memops.h>

/* fnt file magic (OS magic + file format magic) */
Uint8 fnt_magic[24] = 
{
	'A', 'L', 'I', 'C', 'E', 'O', 'S',
	0xE3, 0x81, 0xA7, 0xE3, 0x82, 0x8C, /* Hiragana "dere", UTF-8 */
	'F', 'B', 'F', 'O', 'N', 'T', 'F', 'I', 'L', 'E',
	'\0',
};
/* fnt file header structure */
typedef struct
{
	Uint8 magic[24];
	Uint8 width;
	Uint8 height;
	Uint8 format;
	Uint8 flags;
	Uint32 limit; /* highest defined character (usually 255 for non-unicode fonts) */
} fnt_header_t;
/* header is followed by the raw glyph bitmaps */

/* flag definitions (reserved at the moment) */
#define FLAG_RESV1   1
#define FLAG_RESV2   2
#define FLAG_RESV3   4
#define FLAG_RESV4   8
#define FLAG_RESV5  16
#define FLAG_RESV6  32
#define FLAG_RESV7  64
#define FLAG_RESV8 128

/* load a font from the ramdisk */
Uint8 vga_loadfnt( fnt_t *dest, char *fname )
{
	/* there are many error checks here... */
	if ( !dest )
		return 1;
	rd_entry_t *fnt = rd_find(fname);
	if ( !fnt )
		return 1;
	if ( fnt->size < sizeof(fnt_header_t) )
		return 1;
	Uint8 *fntdata = (Uint8*)rd_find_data(fname);
	/* verify header magic */
	fnt_header_t *hd = (fnt_header_t*)fntdata;
	if ( memcmp(&(hd->magic[0]),&fnt_magic[0],24) )
		return 1;
	/* data retrieval */
	fntdata += sizeof(fnt_header_t);
	dest->data = fntdata;
	dest->w = hd->width;
	dest->h = hd->height;
	dest->flags = hd->flags;
	dest->format = hd->format;
	dest->limit = hd->limit;
	return 0;
}
