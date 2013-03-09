/*
	hcf.c : Halt and catch fire, also known as OHSHI().
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <hcf.h>
#include <sys/regs.h>
#include <sys/helpers.h>
#include <vga/mode3h.h>
#include <vga/vgapal.h>
#include <vga/vgafont.h>
/* for when you screwed up pretty bad */
void OHSHI( char *mesg, regs_t *regs )
{
	int_disable();
	/* fall back to text mode */
	mode_3h.setmode();
	mode_3h.clear();
	mode_3h.setpal(&alicepal[0]);
	setfont(&biosfnt[0]);
	mode_3h.fbsetcursor(0,0);
	/* Yellow on Red means baaaaaad */
	mode_3h.fbsetattr(APAL_YELLOW,APAL_MAROON,EXATTR_NOSCR);
	Uint8 i;
	for ( i=0; i<80; i++ )
		mode_3h.drawchar(i,0,' ');
	for ( i=0; i<80; i++ )
		mode_3h.drawchar(i,24,' ');
	mode_3h.fbprintf("%{1,0CRITICAL ERROR");
	mode_3h.fbprintf("%{-15,CRITICAL ERROR");
	mode_3h.fbprintf("%{1,-1CRITICAL ERROR");
	mode_3h.fbprintf("%{-15,-1CRITICAL ERROR");
	/* no shit sherlock */
	mode_3h.fbprintf("%{1,2%[0FSomething broke! D:%{2,4%[9%s",
			(*mesg)?mesg:HCF_UNHANDLEDINT);
	/* dump */
	mode_3h.fbprintf("%[7%{2,6INTNO %#08x",regs->intno);
	mode_3h.fbprintf("%[7%{19,6ERRNO %#08x",regs->errno);
	mode_3h.fbprintf("%[F%{1,8Registers:");
	mode_3h.fbprintf("%[7%{2,10EAX %#08x",regs->eax);
	mode_3h.fbprintf("%[7%{17,10ECX %#08x",regs->ecx);
	mode_3h.fbprintf("%[7%{32,10EDX %#08x",regs->edx);
	mode_3h.fbprintf("%[7%{47,10EBX %#08x",regs->ebx);
	mode_3h.fbprintf("%[7%{2,11ESP %#08x",regs->esp);
	mode_3h.fbprintf("%[7%{17,11EBP %#08x",regs->ebp);
	mode_3h.fbprintf("%[7%{32,11ESI %#08x",regs->esi);
	mode_3h.fbprintf("%[7%{47,11EDI %#08x",regs->edi);
	mode_3h.fbprintf("%[7%{2,12EIP %#08x",regs->eip);
	mode_3h.fbprintf("%[7%{2,13SS  %#08x",regs->ss);
	mode_3h.fbprintf("%[7%{17,13CS  %#08x",regs->cs);
	mode_3h.fbprintf("%[7%{32,13DS  %#08x",regs->ds);
	mode_3h.fbprintf("%[7%{2,14GS  %#08x",regs->gs);
	mode_3h.fbprintf("%[7%{17,14FS  %#08x",regs->fs);
	mode_3h.fbprintf("%[7%{32,14ES  %#08x",regs->es);
	mode_3h.fbprintf("%[7%{2,16EFLAGS %#08x [",regs->eflags);
	for ( i=0; i<32; i++ )
		mode_3h.fbprintf("%u",regs->eflags>>(31-i)&1);
	mode_3h.fbprintf("]");
	/* heh */
	mode_3h.fbprintf("%[F%{1,-3Computer will now halt and catch fire, have"
			" a nice day! :)");
	khalt(EXIT_EXCEPTION);
}
