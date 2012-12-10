/*
	isr.c : High level interrupt service routines and request handlers.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, The Alice Operating System.
	Released under the MIT License.
*/

#include "isr.h"

void isr_handler( registers_t regs )
{
	puts("received interrupt: ");
	puth(regs.int_no);
	putc('\n');
}
