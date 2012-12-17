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

Uint32 *initial_esp;

/* C entry point for the kernel starts here. */
int kmain( struct multiboot *mboot, Uint32 mboot_mag, Uint32 *esp )
{
	/* set the initial stack pointer */
	initial_esp = esp;
	/* serial output */
	serial_ins();
	printk_s(SERIAL_A, "\033[1;36m%s %s.%s.%s-%s (%s)\033[0m\n",_kname,_kver_maj,_kver_min,_kver_low,_kver_suf,_kver_code);
	/* pretty screen fill */
	vga_clr_s();
	vga_curset(0,0);
	/* rev up those variables! */
	printk_s(SERIAL_A, "Initiating printk demo...\n");
	char chr = 'A';
	Uint64 hex = 0xABADCAFE;
	char *str = "hello world";
	Uint64 oct = 0755;
	Sint64 dec = -2857265;
	vga_setattr(CYAN,BLUE);
	vga_putmc(' ',80);
	printk("%{1,0%s %s.%s.%s-%s (%s)\n",_kname,_kver_maj,_kver_min,_kver_low,_kver_suf,_kver_code);
	vga_setattr(LIGHTGRAY,BLACK);
	printk("%[Fprinting a char at -1,0 (top right corner)%{-1,0%c\n",chr);
	printk("%[7wah, numbers incoming!\n%{4%[A%#3o %d %#8x\n",oct,dec,hex);
	printk("%[3now there will be a string at 0,-1 (bottom left corner)\n%{0,-1%[20%s",str);
	vga_setattr(LIGHTGRAY,BLACK);
	/* THE END */
	printk_s(SERIAL_A, "All done!\n");
	return 0xADEADBED;
}
