/*
	aosrd.h : simple ramdisk for AOS.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/types.h>

/*

Layout of an AOS simple ramdisk

********
*HEADER*
********
|ENTRY|
|ENTRY|
|ENTRY|
|ENTRY|
|ENTRY|
|ENTRY|
| ... |
|ENTRY|
+-----+
xxxxxxxxx
xRAWDATAx
xxxxxxxxx
.TRAILER.
.........

Every ramdisk starts with a header, which contains:
 - a magic number, 0xFEEDCAFE (don't feed the ramdisk cafe, even if it asks)
 - the number of entries in the disk
Exactly right after that (no padding) the entries, one after another, all
packed up. Each entry contains:
 - name of file (256 characters)
 - starting offset in ramdisk where file starts
 - size of file in bytes
After all this, there's a BIG chunk of raw data
As a safeguard, at the end of the whole thing there's a trailer composed of:
 - a magic number, 0xADEADBED (beds can die too)
 - the null-terminated string "The Doll Maker of Bucuresti" (don't ask)
*/

const Uint8 aosrd_hdmagic[4] = {0xFE,0xED,0xCA,0xFE};
const Uint8 aosrd_trmagic[4] = {0xAD,0xEA,0xDB,0xED};
const char aosrd_trsig[28] = "The Doll Maker of Bucuresti";

typedef struct
{
	Uint8 magic[4];
	Uint32 numents;
} rd_header_t;

typedef struct
{
	char name[256];
	Uint32 start;
	Uint32 size;
} rd_entry_t;