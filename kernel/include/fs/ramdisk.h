/*
	ramdisk.h : Functions for handling the ramdisk.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef RAMDISK_H
#define RAMDISK_H
#include <sys/types.h>
/* magic and signatures */
#define AOSRD_HDMAGIC 0xFEEDCAFE
#define AOSRD_TRMAGIC 0xADEADBED
#define AOSRD_TRSIG "The Doll Maker of Bucuresti"
/* header */
#define RDHEAD_SIZ 8
typedef struct
{
	uint32_t magic;
	uint32_t numents;
} rd_header_t;
/* entry */
#define RDENT_SIZ 264
typedef struct
{
	char name[256];
	uint32_t start;
	uint32_t size;
} rd_entry_t;
/* try to find an entry in the ramdisk by filename */
rd_entry_t *rd_find( char *fname );
/* try to find an entry in the ramdisk by filename, return its data address */
uint32_t rd_find_data( char *fname );
/* retrieve an entry by its index number */
rd_entry_t *rd_entry( uint32_t idx );
/* retrive the number of entries in the ramdisk */
uint32_t rd_numents( void );
/* initialize ramdisk handler */
uint8_t init_ramdisk( uint32_t start, uint32_t end );
#endif
