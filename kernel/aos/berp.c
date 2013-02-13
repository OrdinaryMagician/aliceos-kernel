/*
	berp.c : Unrecoverable error handling.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <berp.h>
#include <sys/helpers.h>
#include <printk.h>
#include <serial.h>

/* a kernel panic in the style of a nokinan machine basic error response printer */
extern void berp( const char *message, const char *file, Uint32 line )
{
	/* PANIC HARD */
	printk(SERIAL_A,"%[07ERR INST %s,%u (%s) BERP\n",file,line,message);
	/* stahp */
	asm volatile ("cli");
	asm volatile ("hlt");
}
