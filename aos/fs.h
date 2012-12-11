#ifndef FS_H
#define FS_H
#include "types.h"

#define FS_FILE        0x01
#define FS_DIRECTORY   0x02
#define FS_CHARDEVICE  0x03
#define FS_BLOCKDEVICE 0x04
#define FS_PIPE        0x05
#define FS_SYMLINK     0x06
#define FS_MOUNTPOINT  0x08

struct fs_node;
struct dirent;

typedef Uint32 (*read_type_t)( struct fs_node*, Uint32, Uint32, Uint8* );
typedef Uint32 (*write_type_t)( struct fs_node*, Uint32, Uint32, Uint8* );
typedef void (*open_type_t)( struct fs_node* );
typedef void (*close_type_t)( struct fs_node* );
typedef struct dirent * (*readdir_type_t)( struct fs_node*, Uint32 );
typedef struct fs_node * (*finddir_type_t)( struct fs_node*, char *name );

typedef struct fs_node
{
	char name[256];
	Uint32 mask;
	Uint32 uid;
	Uint32 gid;
	Uint32 flags;
	Uint32 inode;
	Uint32 length;
	Uint32 impl;
	read_type_t read;
	write_type_t write;
	open_type_t open;
	close_type_t close;
	readdir_type_t readdir;
	finddir_type_t finddir;
	struct fs_node *ptr;
} fs_node_t;

struct dirent
{
	char name[256];
	Uint32 ino;
};

extern fs_node_t *fs_root;

Uint32 read_fs( fs_node_t *node, Uint32 offset, Uint32 size, Uint8 *buffer );
Uint32 write_fs( fs_node_t *node, Uint32 offset, Uint32 size, Uint8 *buffer );
void open_fs( fs_node_t *node, Uint8 read, Uint8 write );
void close_fs( fs_node_t *node );
struct dirent *readdir_fs( fs_node_t *node, Uint32 index );
fs_node_t *finddir_fs( fs_node_t *node, char *name );

#endif
