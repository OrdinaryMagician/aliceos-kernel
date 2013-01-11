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
#include <cmos.h>

Uint32 *initial_esp;

/* C entry point for the kernel starts here. */
int kmain( struct multiboot *mboot, Uint32 mboot_mag, Uint32 *esp )
{
	/* set the initial stack pointer */
	initial_esp = esp;
	/* serial output */
	serial_ins();
	printk_s(SERIAL_A,"\033[1;36m%s %s - %s.%s.%s-%s (%s)\033[0m\n",_kname,_kver_code,_kver_maj,_kver_min,_kver_low,_kver_suf,_karch);
	/* pretty screen fill */
	vga_clr_s();
	vga_curset(0,0);
	vga_setcroparea(2,25);	/* only things below the header should scroll */
	/* print header */
	vga_setattr(CYAN,BLUE);
	vga_putmc(' ',80);
	printk("%{1,0%s %s - %s.%s.%s-%s (%s)",_kname,_kver_code,_kver_maj,_kver_min,_kver_low,_kver_suf,_karch);
	vga_curset(0,2);
	vga_setattr(WHITE,BLACK);
	/* a CMOS dump test */
	Uint8 cmosval[128];
	cmos_dump(&cmosval[0]);
	/* pretty print time and date */
	char weekdays[7][4] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	vga_setattr(CYAN,BLUE);
	printk("%{-26,0%s %02x/%02x/20%02x | %02x:%02x:%02x",weekdays[clamp(cmosval[6],1,7)-1],cmosval[7],cmosval[8],cmosval[9],cmosval[4],cmosval[2],cmosval[0]);
	vga_curset(0,2);
	vga_setattr(WHITE,BLACK);
	/* THE END */
	printk_s(SERIAL_A,"All done!\n");
	return 0xADEADBED;
}
