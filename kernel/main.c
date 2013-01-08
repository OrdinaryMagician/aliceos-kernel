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
	printk_s(SERIAL_A,"Initiating printk demo...\n");
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
	printk_s(SERIAL_A,"Finished printk demo.\n");
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
	char tofind_i = 'M';
	char tofind_r = 'o';
	char tofind_ir = 'O';
	char *findtest = "The Quick Brown Fox Jumps Over The Lazy Dog";
	char *compa = "Compare me (a)";
	char *compb = "Compare me (A)";
	char *dst = "overwrite me";
	char *src = "huehuehuehue";
	char empty[256] = "";
	char *conts = "MXZG";
	/* the tests */
	printk_s(SERIAL_A,"\033[1;36mchrlcaps() and chrcaps() test\033[0m\n");
	printk_s(SERIAL_A,"\'%c\' -> \'%c\'\n",capsc,chrlcaps(capsc));
	printk_s(SERIAL_A,"\'%c\' -> \'%c\'\n",lowc,chrcaps(lowc));
	printk_s(SERIAL_A,"\033[1;36mstrlcaps() and strcaps() test\033[0m\n");
	printk_s(SERIAL_A,"original:  \"%s\"\n",convme);
	strlcaps(convme);
	printk_s(SERIAL_A,"lowercase: \"%s\"\n",convme);
	strcaps(convme);
	printk_s(SERIAL_A,"uppercase: \"%s\"\n",convme);
	printk_s(SERIAL_A,"\033[1;36mstrcat() test\033[0m\n");
	printk_s(SERIAL_A,"string a: \"%s\"\n",cata);
	printk_s(SERIAL_A,"string b: \"%s\"\n",catb);
	strcat(cata,catb);
	printk_s(SERIAL_A,"result:   \"%s\"\n",cata);
	printk_s(SERIAL_A,"\033[1;36mstrchr() test\033[0m\n");
	printk_s(SERIAL_A,"will look for the character \'%c\'\n",tofind);
	printk_s(SERIAL_A,"inside of \"%s\"\n",findtest);
	char *found = strchr(findtest,tofind);
	if ( *found )
		printk_s(SERIAL_A,"the character was found at offset %d\n",(found-findtest));
	else
		printk_s(SERIAL_A,"the character wasn't found\n");
	printk_s(SERIAL_A,"\033[1;36mstrcasechr() test\033[0m\n");
	printk_s(SERIAL_A,"will look for the character \'%c\'\n",tofind_i);
	printk_s(SERIAL_A,"inside of \"%s\"\n",findtest);
	found = strcasechr(findtest,tofind_i);
	if ( *found )
		printk_s(SERIAL_A,"the character was found at offset %d\n",(found-findtest));
	else
		printk_s(SERIAL_A,"the character wasn't found\n");
	printk_s(SERIAL_A,"\033[1;36mstrrchr() test\033[0m\n");
	printk_s(SERIAL_A,"will look for the character \'%c\'\n",tofind_r);
	printk_s(SERIAL_A,"inside of \"%s\"\n",findtest);
	found = strrchr(findtest,tofind_r);
	if ( *found )
		printk_s(SERIAL_A,"the character was found at offset %d\n",(found-findtest));
	else
		printk_s(SERIAL_A,"the character wasn't found\n");
	printk_s(SERIAL_A,"\033[1;36mstrcaserchr() test\033[0m\n");
	printk_s(SERIAL_A,"will look for the character \'%c\'\n",tofind_ir);
	printk_s(SERIAL_A,"inside of \"%s\"\n",findtest);
	found = strrcasechr(findtest,tofind_ir);
	if ( *found )
		printk_s(SERIAL_A,"the character was found at offset %d\n",(found-findtest));
	else
		printk_s(SERIAL_A,"the character wasn't found\n");
	printk_s(SERIAL_A,"\033[1;36mstrcmp() and strcasecmp() test\033[0m\n");
	printk_s(SERIAL_A,"will compare the following strings\n");
	printk_s(SERIAL_A,"string a: \"%s\"\n",compa);
	printk_s(SERIAL_A,"string b: \"%s\"\n",compb);
	if ( !strcmp(compa,compb) )
		printk_s(SERIAL_A,"the strings are equal\n");
	else
		printk_s(SERIAL_A,"the strings are different\n");
	printk_s(SERIAL_A,"now for a case-insensitive compare\n");
	if ( !strcasecmp(compa,compb) )
		printk_s(SERIAL_A,"the strings are equal\n");
	else
		printk_s(SERIAL_A,"the strings are different\n");
	printk_s(SERIAL_A,"\033[1;36mstrcpy() test\033[0m\n");
	printk_s(SERIAL_A,"will overwrite\n");
	printk_s(SERIAL_A,"\"%s\"\n",dst);
	printk_s(SERIAL_A,"with\n");
	printk_s(SERIAL_A,"\"%s\"\n",src);
	printk_s(SERIAL_A,"result: \"%s\"\n",strcpy(dst,src));
	printk_s(SERIAL_A,"\033[1;36mstrcont() and strcasecont() test\033[0m\n");
	printk_s(SERIAL_A,"Does \"%s\" contain any of the characters in the set \"%s\"?\n",findtest,conts);
	printk_s(SERIAL_A,"case-sensitive: ");
	if ( !strcont(findtest,conts) )
		printk_s(SERIAL_A,"yes\n");
	else
		printk_s(SERIAL_A,"no\n");
	printk_s(SERIAL_A,"case-insensitive: ");
	if ( !strcasecont(findtest,conts) )
		printk_s(SERIAL_A,"yes\n");
	else
		printk_s(SERIAL_A,"no\n");
	printk_s(SERIAL_A,"\033[1;36mstrlen() test\033[0m\n");
	printk_s(SERIAL_A,"Length of \"%s\": %d\n",findtest,strlen(findtest));
	printk_s(SERIAL_A,"Finished strops demo...\n");
}

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
	/* run demos */
	printk_demo();
	strops_demo();
	/* THE END */
	printk_s(SERIAL_A,"All done!\n");
	return 0xADEADBED;
}
