/*
	fs.c : Virtual filesystem.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include "fs.h"

fs_node_t *fs_root = 0;

Uint32 read_fs( fs_node_t *node, Uint32 offset, Uint32 size, Uint8 *buffer )
{
	if ( node->read != 0 )
		return node->read(node,offset,size,buffer);
	return 0;
}

Uint32 write_fs( fs_node_t *node, Uint32 offset, Uint32 size, Uint8 *buffer )
{
	if ( node->write != 0 )
		return node->write(node,offset,size,buffer);
	return 0;
}

void open_fs( fs_node_t *node, Uint8 read, Uint8 write )
{
	if ( node->open != 0 )
		node->open(node);
}

void close_fs( fs_node_t *node )
{
	if ( node->close != 0 )
		node->close(node);
}

struct dirent *readdir_fs( fs_node_t *node, Uint32 index )
{
	if ( ((node->flags&0x07) == FS_DIRECTORY) && (node->readdir != 0) )
		return node->readdir(node,index);
	return 0;
}

fs_node_t *finddir_fs( fs_node_t *node, char *name )
{
	if ( ((node->flags&0x07) == FS_DIRECTORY) && (node->readdir != 0) )
		return node->finddir(node,name);
	return 0;
}
