#ifndef INITRD_H
#define INITRD_H
#include "types.h"
#include "fs.h"
typedef struct
{
	Uint32 nfiles;
} initrd_header_t;
typedef struct
{
	Uint8 magic;
	char name[64];
	Uint32 offset;
	Uint32 length;
} initrd_file_header_t;
fs_node_t *init_initrd( Uint32 location );
#endif
