/*
	kmain.c : C entry point for the AliceOS kernel.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include "aos/desc.h"
#include "aos/screen.h"
#include "aos/string.h"
#include "aos/timer.h"
#include "aos/panic.h"
#include "aos/paging.h"
#include "aos/fs.h"
#include "aos/initrd.h"
#include "aos/multiboot.h"

#define KNAME "AliceOS"
#define KVER "0.0.1a"

extern Uint32 placement_address;

void drawheader()
{
	Uint8 ofg, obg;
	int ox, oy, i;
	text_getcolor(&ofg,&obg);
	cur_get(&ox,&oy);
	text_setcolor(LGRAY,BLUE);
	for( i=0; i<80; i++ )
		putc(' ');
	text_setcolor(LCYAN,BLUE);
	cur_set(1,0);
	puts(KNAME);
	cur_set(-(1+strlen(KVER)),0);
	puts(KVER);
	text_setcolor(ofg,obg);
	cur_set(ox,oy);
}

void lsroot( void )
{
	int i = 0;
	struct dirent *node = 0;
	while ( (node = readdir_fs(fs_root,i)) != 0 )
	{
		puts("Found file ");
		puts(node->name);
		fs_node_t *fsnode = finddir_fs(fs_root,node->name);
		if ( (fsnode->flags&0x07) == FS_DIRECTORY )
			puts("\n\t(directory)\n");
		else
		{
			puts("\n\tsize: ");
			putlu(fsnode->length);
			puts(" bytes");
			char buf[256];
			Uint32 sz = read_fs(fsnode,0,256,(Uint8*)buf);
			int j;
			puts("\n\tcontents: \"");
			for ( j=0; j<sz; j++ )
				putc(buf[j]);
			puts("\"\n");
		}
		i++;
	}
}

int kmain( struct multiboot *mboot_ptr )
{
	init_descriptor_tables();
	init_video();
	ASSERT(mboot_ptr->mods_count > 0);
	Uint32 initrd_location = *((Uint32*)mboot_ptr->mods_addr);
	Uint32 initrd_end = *(Uint32*)(mboot_ptr->mods_addr+4);
	placement_address = initrd_end;
	init_paging();
	fs_root = init_initrd(initrd_location);
	drawheader();
	cur_set(0,2);
	puts("AliceOS kernel loaded. Hello world!\n");
	puts("Listing root directory...\n");
	lsroot();

	return 0xADEADBED;
}
