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

void printk_demo( void )
{
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
	printk_s(SERIAL_A, "Finished printk demo.\n");
}

void strops_demo( void )
{
	/* some vars for testing */
	printk_s(SERIAL_A, "Initiating strops demo...\n");
	char lowc = 'e';
	char capsc = 'G';
	char *convme = "Convert Me to CAPS or lowercase.";
	char cata[256] = "place another string here -> ";
	char *catb = "append me";
	char tofind = 'x';
	char *findtest = "The Quick Brown Fox Jumps Over The Lazy Dog";
	char *compa = "Compare me (A)";
	char *compb = "Compare me (B)";
	char *dst = "overwrite me";
	char *src = "huehuehuehue";
	char empty[256] = "";
	/* the tests */
	printk_s(SERIAL_A, "\033[1;36mchrlcaps() and chrcaps() test\033[0m\n");
	printk_s(SERIAL_A, "\'%c\' -> \'%c\'\n",capsc,chrlcaps(capsc));
	printk_s(SERIAL_A, "\'%c\' -> \'%c\'\n",lowc,chrcaps(lowc));
	printk_s(SERIAL_A, "\033[1;36mstrlcaps() and strcaps() test\033[0m\n");
	printk_s(SERIAL_A, "original:  \"%s\"\n",convme);
	strlcaps(convme);
	printk_s(SERIAL_A, "lowercase: \"%s\"\n",convme);
	strcaps(convme);
	printk_s(SERIAL_A, "uppercase: \"%s\"\n",convme);
	printk_s(SERIAL_A, "\033[1;36mstrcat() test\033[0m\n");
	printk_s(SERIAL_A, "string a: \"%s\"\n",cata);
	printk_s(SERIAL_A, "string b: \"%s\"\n",catb);
	strcat(cata,catb);
	printk_s(SERIAL_A, "result:   \"%s\"\n",cata);
	printk_s(SERIAL_A, "\033[1;36mstrchr() test\033[0m\n");
	printk_s(SERIAL_A, "will look for the character \'%c\'\n",tofind);
	printk_s(SERIAL_A, "inside of \"%s\"\n",findtest);
	
	printk_s(SERIAL_A, "Finished strops demo...\n");
}

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
	/* run demos */
	printk_demo();
	strops_demo();
	/* THE END */
	printk_s(SERIAL_A, "All done!\n");
	return 0xADEADBED;
}
