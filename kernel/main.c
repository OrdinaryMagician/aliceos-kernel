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
#include <kmem.h>

Uint32 *initial_esp;
/* location of certain parts of the kernel */
extern Uint32 code;
extern Uint32 data;
extern Uint32 bss;
extern Uint32 end;

static void init_serial( void )
{
	serial_on(SERIAL_A);
	printk("\n\033[1;36m%s %s %s.%s.%s%s %s %s %s %s\033[0m\n\n",
		_kname,_kver_code,_kver_maj,_kver_min,_kver_low,_kver_suf,
		_kbuild_date,_kbuild_time,_karch,_kosname);
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
	mode_3h.fbsetattr(APAL_WHITE,APAL_BLACK,0);
}

static void draw_info( void )
{
	mode_3h.fbsetattr(APAL_CYAN,APAL_BLACK,0);
	/* some kind of uname */
	mode_3h.fbprintf("%s %s %s.%s.%s%s %s %s %s %s\n\n",
			_kname,_kver_code,_kver_maj,_kver_min,_kver_low,
			_kver_suf,_kbuild_date,_kbuild_time,_karch,_kosname);
	mode_3h.fbsetattr(APAL_WHITE,APAL_BLACK,0);
}

/* C entry point for the kernel starts here. */
int kmain( multiboot_t *mboot, Uint32 mboot_mag, Uint32 *esp )
{
	/* set the initial stack pointer */
	initial_esp = esp;

	/* stuff to save */
	char kfname[32];
	char kargs[KCMDLINE_MAX];
	char kramdisk[32];
	
	/* ramdisk module */
	mbootmod_t *rdisk = NULL;

	/* saving */
	char *kargs_r = strtok((char*)mboot->cmdline," ");
	strncpy(kargs,kargs_r,KCMDLINE_MAX-1);
	strncpy(kfname,(char*)mboot->cmdline,min(31,strlen((char*)mboot->cmdline)));
	if ( mboot->mods_count )
	{
		rdisk = (mbootmod_t*)mboot->mods_addr;
		strncpy(kramdisk,(char*)rdisk->cmdline,31);
	}

	/* for pretty-printing */
	char *left = " \033[1;37m>\033[1;36m>\033[0;36m>\033[0m ";
	
	/* calculate placement address for the memory allocator */
	Uint32 p_addr = end;
	/* skip ramdisk if loaded */
	if ( rdisk )
		p_addr = rdisk->mod_end;

	/* kernel signature is located right at the very beginning */
	char *ksig = (char*)&code;
	/* start stuff */
	init_serial();
	printk("Kernel: %s loaded at %#08x\n",kfname,&code);
	printk(" 2-part signature:\n");
	printk("  \033[1;32m%s\033[0m\n",ksig);
	printk("  \033[1;32m%s\033[0m\n",ksig+32);
	if ( kargs[0] )
		printk(" Kernel arguments: %s\n",kargs);
	if ( rdisk )
		printk("Ramdisk: %s loaded at %#08x\n",kramdisk,rdisk->mod_start);
	printk("Starting up...\n");
	printk("%sMemory manager\n",left);
	init_kmem(p_addr);
	printk("%sConsole output\n",left);
	init_console();
	printk("%sDescriptor tables\n",left);
	init_descriptor_tables();
	printk("%sInternal timer\n",left);
	init_timer(TIMER_HZ);
	draw_info();
	if ( rdisk && init_ramdisk(rdisk->mod_start,rdisk->mod_end) )
		BERP("Ramdisk initialization failed");

	/* insert demo code or whatever here */
	if ( !kargs[0] )
	{
		mode_3h.fbputs("No argument specified\n\n");
		listdemos();
		return EXIT_SUCCESS;
	}
	unsigned int i = 0;
	for ( i=0; i<DEMO_COUNT; i++ )
	{
		if ( !strcmp(kargs,demos[i].name) )
		{
			demos[i].func();
			return EXIT_SUCCESS;
		}
	}
	mode_3h.fbprintf("Unrecognized argument \"%s\"\n\n",kargs);
	listdemos();

	return EXIT_SUCCESS;
}
