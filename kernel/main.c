/*
	kmain.c : C entry point for the AliceOS kernel.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/types.h>
#include <sys/helpers.h>
#include <sys/multiboot.h>
#include <sys/serial.h>
#include <sys/desc_tables.h>
#include <sys/timer.h>
#include <sys/paging.h>
#include <sys/kbd.h>
#include <sys/task.h>
#include <vga/vgapal.h>
#include <vga/vgafont.h>
#include <vga/vgamisc.h>
#include <vga/mode3h.h>
#include <fs/ramdisk.h>
#include <sh/shell.h>
#include <kdefs.h>
#include <printk.h>
#include <berp.h>
#include <strops.h>
#include <kmem.h>
#include <sys/pci.h>
/* initial stack pointer */
uint32_t *initial_esp;
/* location of certain parts of the kernel */
extern uint32_t code;
extern uint32_t data;
extern uint32_t bss;
extern uint32_t end;
/* C entry point for the kernel starts here. */
int kmain( multiboot_t *mboot, uint32_t mboot_mag, uint32_t *esp )
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
	char *left = " \033[1;37m>\033[1;36m>\033[0;36m>\033[0m ";
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
	printk("%sMemory manager\n",left);
	/* extra gap for the memory allocator */
	uint32_t gap_st = 0;
	uint32_t gap_en = (rdisk)?rdisk->mod_end:(uint32_t)&end;
	kmem_addgap(gap_st,gap_en);
	/* placement address and memory end */
	uint32_t paddr = gap_en;
	uint32_t eaddr = 0x1000000;	/* 16MiB by default */
	/* add mmap entries too */
	mmap_entry_t *mmapent = (mmap_entry_t*)mboot->mmap_addr;
	uint32_t off = 0;
	uint32_t offmax = mboot->mmap_length;
	while ( off < offmax )
	{
		gap_st = mmapent->addr_l;
		gap_en = mmapent->addr_l+mmapent->len_l;
		if ( mmapent->type != 1 )
		{
			kmem_addgap(gap_st,gap_en);
			/* the highest mmap entry defines memory limit */
			eaddr = (gap_en>eaddr)?gap_en:eaddr;
		}
		off += mmapent->size+4;
		mmapent = (mmap_entry_t*)(mboot->mmap_addr+off);
	}
	/* finally, initialize the manager */
	init_kmem(paddr,eaddr);
	/* descriptor tables and interrupts */
	printk("%sDescriptor tables\n",left);
	init_descriptor_tables();
	/* paging */
	printk("%sPaging and dynamic memory allocation\n",left);
	init_paging();
	/* internal timer */
	printk("%sInternal timer at %uhz",left,TIMER_HZ);
	init_timer(TIMER_HZ);
	int32_t error = TIMER_HZ-get_hz();
	printk(" (~%uhz error)\n",abs(error));
	/* multitasking */
	printk("%sMultitasking\n",left);
	init_tasking();
	/* keyboard input */
	//printk("%sPS/2 keyboard input driver\n",left);
	//kbd_on();
	/* PCI stuff */
	//printk("%sEnumerating PCI devices\n",left);
	//init_pci();
	/* set up ramdisk access */
	if ( rdisk && init_ramdisk(rdisk->mod_start,rdisk->mod_end) )
		BERP("Ramdisk initialization failed");
	/* print some kind of uname */
	mode_3h.fbsetattr(APAL_CYAN,APAL_BLACK,0);
	mode_3h.fbprintf("%s %s %s.%s.%s%s %s %s %s %s\n\n",
			_kname,_kver_code,_kver_maj,_kver_min,_kver_low,
			_kver_suf,_kbuild_date,_kbuild_time,_karch,_kosname);
	mode_3h.fbsetattr(APAL_WHITE,APAL_BLACK,0);
	/* RAM amount */
	uint8_t mult = 0;
	char suf[7][3] = {"","Ki","Mi","Gi","Ti","Pi","Ei"};
	uint32_t cmem = eaddr;
	while ( mult<7 )
	{
		cmem = eaddr;
		eaddr >>= 10;
		if ( !eaddr )
			break;
		mult++;
	}
	printk("Total RAM: %u %sB\n",cmem,suf[mult]);
	/* call internal shell */
	/*if ( fork() )
	{
		printk("Started shell with PID %u\n",getpid());
		shell.init();
		shell.enable();
		return EXIT_SUCCESS;
	}*/
	/*int ret;
	ret = fork();
	printk("fork() returned %u, getpid() returns %u\n",ret,getpid());*/
	return EXIT_SUCCESS;
}
