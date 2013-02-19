/*
	kmain.c : C entry point for the AliceOS kernel.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/types.h>
#include <sys/helpers.h>
#include <kdefs.h>
#include <sys/multiboot.h>
#include <printk.h>
#include <sys/serial.h>
#include <sys/cmos.h>
#include <berp.h>
#include <strops.h>
#include <demos.h>
#include <fs/ramdisk.h>
#include <vga/vgapal.h>
#include <vga/vgafont.h>
#include <vga/mode3h.h>
#include <sys/desc_tables.h>
#include <sys/timer.h>

Uint32 *initial_esp;

static void init_serial( void )
{
	serial_ins();
	printk(SERIAL_A,"\033[1;36m%s %s - %s.%s.%s%s (%s)\033[0m\n",_kname,_kver_code,_kver_maj,_kver_min,_kver_low,_kver_suf,_karch);
}

static void init_console( void )
{
	/* initialize some vga settings */
	mode_3h.setmode();
	initfont();
	mode_3h.setpal(&alicepal[0]);
	/* pretty screen fill */
	mode_3h.clear();
	mode_3h.fbcursorvis(1);
	mode_3h.fbsetcursor(0,0);
}

static void draw_header( void )
{
	mode_3h.fbsetattr(APAL_CYAN,APAL_BLUE,0);
	int i = 0;
	while ( i < 80 )
		mode_3h.drawchar(i++,0,' ');
	mode_3h.fbprintf("%{1,0%s %s - %s.%s.%s%s (%s)",_kname,_kver_code,_kver_maj,_kver_min,_kver_low,_kver_suf,_karch);
	/* pretty print time and date */
	Uint8 cmosval[128];
	cmos_dump(&cmosval[0]);
	char weekdays[7][4] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	mode_3h.fbsetattr(APAL_CYAN,APAL_BLUE,0);
	mode_3h.fbprintf("%{-26,0%s %02x/%02x/20%02x | %02x:%02x:%02x",weekdays[clamp(cmosval[6],1,7)-1],cmosval[7],cmosval[8],cmosval[9],cmosval[4],cmosval[2],cmosval[0]);
	mode_3h.fbsetcursor(0,2);
	mode_3h.fbsetattr(APAL_WHITE,APAL_BLACK,0);
}

/* C entry point for the kernel starts here. */
int kmain( struct multiboot *mboot, Uint32 mboot_mag, Uint32 *esp )
{
	/* set the initial stack pointer */
	initial_esp = esp;

	/* start stuff */
	init_serial();
	init_console();
	init_descriptor_tables();
	draw_header();
	init_timer(TIMER_HZ);
	if ( mboot->mods_count )
		init_ramdisk(*((Uint32*)mboot->mods_addr),*(Uint32*)(mboot->mods_addr+4));

	/* insert demo code or whatever here */
	char *kargs = strchr((char*)mboot->cmdline,' ');
	kargs++;
	if ( !(*kargs) )
	{
		mode_3h.fbputs("No argument specified\n\n");
		listdemos();
		return 0xADEADBED;
	}
	unsigned int i = 0;
	for ( i=0; i<DEMO_COUNT; i++ )
	{
		if ( !strcmp(kargs,demos[i].name) )
		{
			demos[i].func();
			return 0xADEADBED;
		}
	}
	mode_3h.fbprintf("Unrecognized argument \"%s\"\n\n",kargs);
	listdemos();

	return 0xADEADBED;
}
