/*
	main.c : C entry point for the AliceOS kernel.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include "aos/screen.h"

int kmain( struct multiboot *mboot_ptr )
{
	init_video();
	text_setcolor(LCYAN,BLACK);
	puts("AliceOS");
	putc(' ');
	text_setcolor(LBLUE,BLACK);
	puts("v0.0.1");
	putc('\n');
	text_setcolor(DGRAY,BLACK);
	puts("--------------");
	putc('\n');
	text_setcolor(LGRAY,BLACK);
	for(;;);
	return 0xADEADBED;
}
