/*
	kmain.c : C entry point for the AliceOS kernel.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/types.h>
#include <sys/multiboot.h>
#include <video/loadfnt.h>
#include <video/loadimg.h>
#include <vga/struct.h>
#include <vga/mode3h.h>
#include <vga/vgamisc.h>
#include <vga/vgafont.h>
#include <vga/vgapal.h>
#include <kdefs.h>
#include <strops.h>
#include <sys/helpers.h>
#include <sys/serial.h>
#include <printk.h>
#include <kmem.h>
#include <sys/desc_tables.h>
#include <sys/regs.h>
#include <sys/timer.h>
#include <sys/paging.h>
#include <sys/task.h>
#include <sys/kbd.h>
#include <sys/pci.h>
#include <fs/ramdisk.h>
#include <berp.h>
#include <sh/shell.h>
#include <sys/map.h>
/* initial stack pointer */
static uint32_t *initial_esp;
/* location of certain parts of the kernel */
extern uint32_t code;
extern uint32_t data;
extern uint32_t bss;
extern uint32_t end;
/* C entry point for the kernel starts here. */
uint32_t kmain( multiboot_t *mboot, uint32_t mboot_mag, uint32_t *esp )
{
	/* set the initial stack pointer */
	initial_esp = esp;
	/* initialize some vga settings */
	mode_3h.setmode();
	disableblink();
	initfont();
	mode_3h.setpal(&alicepal[0]);
	/* pretty screen fill */
	mode_3h.clear();
	mode_3h.fbcursorvis(1);
	mode_3h.fbsetcursor(0,0);
	mode_3h.fbsetattr(APAL_WHITE,APAL_BLACK,0);
	/* sanity check */
	if ( (mboot_mag != 0x2BADB002) || !mboot )
		BERP("Incorrect multiboot parameters");
	/* stuff to save */
	char kfname[32];
	char kargs[KCMDLINE_MAX];
	char kramdisk[32];
	/* ramdisk module */
	mbootmod_t *rdisk = NULL;
	/* saving */
	char *kargs_r = strtok((char*)mboot->cmdline," ");
	strncpy(kargs,kargs_r,KCMDLINE_MAX-1);
	strncpy(kfname,(char*)mboot->cmdline,
		min(31,strlen((char*)mboot->cmdline)));
	if ( mboot->mods_count )
	{
		rdisk = (mbootmod_t*)mboot->mods_addr;
		strncpy(kramdisk,(char*)rdisk->cmdline,31);
	}
	/* for pretty-printing */
	char *left = "\033[1;37m>\033[1;36m>\033[0;36m>\033[0m ";
	/* kernel signature is located right at the very beginning */
	char *ksig = (char*)&code;
	/* serial logging */
	serial_on(SERIAL_A);
	/* pseudo-uname over serial */
	printk("\n\033[1;36m%s %s %s.%s.%s%s %s %s %s %s\033[0m\n\n",
		_kname,_kver_code,_kver_maj,_kver_min,_kver_low,_kver_suf,
		_kbuild_date,_kbuild_time,_karch,_kosname);
	/* kernel and ramdisk identification */
	printk("Kernel: %s loaded at %#08x\n",kfname,&code);
	printk(" 2-part signature:\n");
	printk("  \033[1;32m%s\033[0m\n",ksig);
	printk("  \033[1;32m%s\033[0m\n",ksig+32);
	if ( kargs[0] )
		printk(" Kernel arguments: %s\n",kargs);
	if ( rdisk )
		printk("Ramdisk: %s loaded at %#08x\n",
			kramdisk,rdisk->mod_start);
	/* loading "modules" */
	printk("Starting up...\n");
	/* memory */
	printk("%sStatic memory manager (placement allocator)\n",left);
	/* extra gap for the memory allocator */
	uint32_t gap_st = 0;
	uint32_t gap_en = (rdisk)?rdisk->mod_end:(uint32_t)&end;
	kmem_addgap(gap_st,gap_en);
	/* placement address and memory end */
	uint32_t paddr = gap_en;
	/* memory end is basically upper memory plus 1MiB */
	uint32_t eaddr = (mboot->mem_upper+1024)*1024; /* convert to bytes */
	/* add mmap entries too */
	mmap_entry_t *mmapent = (mmap_entry_t*)mboot->mmap_addr;
	uint32_t off = 0;
	uint32_t offmax = mboot->mmap_length;
	printk("Memory map\n");
	while ( off < offmax )
	{
		gap_st = mmapent->addr_l;
		gap_en = mmapent->addr_l+mmapent->len_l;
		if ( mmapent->type != 1 )
		{
			printk(" [%#08x-%#08x] reserved\n",gap_st,gap_en);
			kmem_addgap(gap_st,gap_en);
		}
		else
			printk(" [%#08x-%#08x] usable\n",gap_st,gap_en);
		off += mmapent->size+4;
		mmapent = (mmap_entry_t*)(mboot->mmap_addr+off);
	}
	/* finally, initialize the manager */
	init_kmem(paddr,eaddr);
	/* descriptor tables and interrupts */
	printk("%sDescriptor tables and interrupts\n",left);
	init_descriptor_tables();
	/* internal timer */
	printk("%sSystem timer\n",left);
	init_timer(TIMER_HZ);
	/* paging */
	printk("%sPaging and dynamic memory management\n",left);
	init_paging();
	/* multitasking */
	printk("%sMultitasking\n",left);
	init_tasking();
	/* keyboard input */
	printk("%sPS/2 keyboard input driver\n",left);
	kbd_on();
	/* PCI stuff */
	printk("%sEnumerating PCI devices\n",left);
	init_pci();
	/* set up ramdisk access */
	if ( rdisk && init_ramdisk(rdisk->mod_start,rdisk->mod_end) )
		BERP("Ramdisk initialization failed");
	/* system map */
	printk("%sLoading kernel symbol map\n",left);
	init_sysmap();
	/* print some kind of uname */
	mode_3h.fbsetattr(APAL_CYAN,APAL_BLACK,0);
	mode_3h.fbprintf("%s %s %s.%s.%s%s %s %s %s %s\n\n",
			_kname,_kver_code,_kver_maj,_kver_min,_kver_low,
			_kver_suf,_kbuild_date,_kbuild_time,_karch,_kosname);
	mode_3h.fbsetattr(APAL_WHITE,APAL_BLACK,0);
	/* RAM amount */
	printk("Total RAM: %u Bytes\n",eaddr);
	/* Boot time counter */
	uint32_t tt = get_ticks();
	uint32_t tms = (get_timescale()*tt)/1000000;
	uint32_t ts = tms/1000;
	tms %= 1000;
	printk("Booting took %u.%03u seconds (%u ticks)\n",ts,tms,tt);
	printk("Kernel core with PID %u\n",getpid());
	/* call internal shell */
	shell.init();
	shell.enable();
	return EXIT_SUCCESS;
}
