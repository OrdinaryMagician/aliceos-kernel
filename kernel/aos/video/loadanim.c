/*
	loadanim.c : Animated Image loading.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <video/loadanim.h>
#include <fs/ramdisk.h>
#include <memops.h>

/* anm file magic (OS magic  file format magic) */
static Uint8 anm_magic[24] =
{
	'A', 'L', 'I', 'C', 'E', 'O', 'S',
	0xE3, 0x81, 0xA7, 0xE3, 0x82, 0x8C, /* Hiragana "dere", UTF-8 */
	'R', 'A', 'W', 'A', 'N', 'M', 'F', 'I', 'L', 'E',
	'\0',
};
/* anm file header structure */
typedef struct
{
	Uint8 magic[24];
	Uint8 flags;
	Uint8 depth;
	Uint16 width;
	Uint16 height;
	Uint16 palsz;
} anm_header_t;
/* header is followed by the palette (if available), then first frame */

/* load an animation from the ramdisk */
Uint8 loadanm( anm_t *dest, char *fname )
{
	/* there are many error checks here... */
	if ( !dest )
		return 1;
	rd_entry_t *anm = rd_find(fname);
	if ( !anm )
		return 1;
	if ( anm->size < sizeof(anm_header_t) )
		return 1;
	Uint8 *anmdata = (Uint8*)rd_find_data(fname);
	/* verify header magic */
	anm_header_t *hd = (anm_header_t*)anmdata;
	if ( memcmp(&(hd->magic[0]),&anm_magic[0],24) )
		return 1;
	/* first frame retrieval */
	anmdata += sizeof(anm_header_t);
	if ( !(hd->flags&ANM_FL_NOPAL) )
	{
		dest->pal = anmdata;
		anmdata += hd->palsz;
	}
	dest->frames = (anm_frame_t*)anmdata;
	dest->w = hd->width;
	dest->h = hd->height;
	dest->flags = hd->flags;
	dest->depth = hd->depth;
	return 0;
}
