/*
	berp.c : Unrecoverable error handling.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <berp.h>

/* a kernel panic in the style of a nokinan machine basic error response printer */
extern void berp( const char *message, const char *file, Uint32 line )
{
	/* return to text mode just in case */
	vga_modeset(MODE_3H);
	vga_setfont(&biosfnt[0]);
	vga_setpal(&alicepal[0]);
	/* clear screen and move to the top */
	vga_clr_s();
	vga_curset(0,0);
	vga_showcursor();
	/* PANIC HARD */
	printk("%[07ERR INST %s,%u (%s) BERP\n",file,line,message);
	printk_s(SERIAL_A,"\033[1;31;40mERR INST %s,%u (%s) BERP\033[0m\n",file,line,message);
	/* stahp */
	asm volatile ("cli");
	asm volatile ("hlt");
}
