/*
	berp.c : Unrecoverable error handling.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <berp.h>

/* a kernel panic in the style of a nokinan machine basic error response printer */
extern void berp( const char *message, const char *file, Uint32 line )
{
	printk("%[0CERR INST %s,%u (%s) BERP%[0F\n",file,line,message);
	printk_s(SERIAL_A,"\033[1;31;40mERR INST %s,%u (%s) BERP\033[0m\n",file,line,message);
	asm volatile ("cli");
	asm volatile ("hlt");
}
