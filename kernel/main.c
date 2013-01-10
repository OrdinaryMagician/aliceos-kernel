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
#include <printk.h>
#include <serial.h>
#include <strops.h>

Uint32 *initial_esp;

/* C entry point for the kernel starts here. */
int kmain( struct multiboot *mboot, Uint32 mboot_mag, Uint32 *esp )
{
	/* set the initial stack pointer */
	initial_esp = esp;
	/* serial output */
	serial_ins();
	printk_s(SERIAL_A,"\033[1;36m%s %s.%s.%s-%s (%s)\033[0m\n",_kname,_kver_maj,_kver_min,_kver_low,_kver_suf,_kver_code);
	/* pretty screen fill */
	vga_clr_s();
	vga_curset(0,0);
	vga_setcroparea(2,25);	/* only things below the header should scroll */
	/* print header */
	vga_setattr(CYAN,BLUE);
	vga_putmc(' ',80);
	printk("%{1,0%s %s.%s.%s-%s (%s)\n",_kname,_kver_maj,_kver_min,_kver_low,_kver_suf,_kver_code);
	vga_curset(0,2);
	/* stuff */
	vga_setattr(WHITE,BLACK);
	for ( ; ; )
	{
		vga_setattr(WHITE,krand()&0x0F);
		vga_putc(' ');
	}
	/* THE END */
	printk_s(SERIAL_A,"All done!\n");
	return 0xADEADBED;
}
