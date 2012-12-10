/*
	main.c : C entry point for the AliceOS kernel.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include "aos/screen.h"

#define KNAME "AliceOS"
#define KVER "0.0.1a"

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

int kmain( struct multiboot *mboot_ptr )
{
	init_video();
	drawheader();
	cur_set(1,2);
	puts("AliceOS kernel loaded. Hello world!");
	for(;;);
	return 0xADEADBED;
}
