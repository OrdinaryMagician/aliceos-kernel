/*
	kmain.c : C entry point for the AliceOS kernel.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <types.h>
#include <helpers.h>
#include <kdefs.h>
#include <vgatext.h>
#include <multiboot.h>
#include <krand.h>

Uint32 *initial_esp;

/* C entry point for the kernel starts here. */
int kmain( struct multiboot *mboot, Uint32 mboot_mag, Uint32 *esp )
{
	/* set the initial stack pointer */
	initial_esp = esp;
	/* pretty screen fill */
	vga_clr_s();
	vga_curset(0,0);
	int i = 0;
	for ( i=0; i<(80*25); i++ )
	{
		vga_setc(i,(i%2)?'/':'\\',0x07);
	}
	/* THE END */
	return 0xADEADBED;
}
