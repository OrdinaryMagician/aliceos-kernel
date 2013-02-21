/*
	berp.c : Unrecoverable error handling.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <berp.h>
#include <sys/helpers.h>
#include <printk.h>
#include <vga/mode3h.h>
#include <vga/vgapal.h>
#include <vga/vgafont.h>

/* a kernel panic in the style of a nokinan machine basic error response printer */
void berp( const char *message, const char *file, Uint32 line )
{
	int_disable();
	/* fall back to text mode */
	mode_3h.setmode();
	mode_3h.clear();
	mode_3h.setpal(&alicepal[0]);
	setfont(&biosfnt[0]);
	mode_3h.fbsetcursor(0,0);
	/* PANIC HARD */
	mode_3h.fbsetattr(APAL_LIGHTGREEN,APAL_BLACK,EXATTR_NOSCR);
	mode_3h.fbputs("BERP");
	printk("\033[0;5;31mERR INST %s,%u (%s) BERP\033[0m\n",file,line,message);
	/* stahp */
	khalt(EXIT_BERP);
}
