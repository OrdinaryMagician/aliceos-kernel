/*
	isr.c : High level interrupt service routines and request handlers.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, The Alice Operating System.
	Released under the MIT License.
*/

#include "isr.h"
#include "screen.h"
#include "port.h"

isr_t interrupt_handlers[256];

void register_interrupt_handler( Uint8 n, isr_t handler )
{
	interrupt_handlers[n] = handler;
}

void isr_handler( registers_t regs )
{
	puts("received interrupt: ");
	puth(regs.int_no);
	putc('\n');
	if ( interrupt_handlers[regs.int_no] != 0 )
	{
		isr_t handler = interrupt_handlers[regs.int_no];
		handler(regs);
	}
}

void irq_handler( registers_t regs )
{
	if ( regs.int_no >= 40 )
		outb(0xA0,0x20);
	outb(0x20,0x20);
	if ( interrupt_handlers[regs.int_no] != 0 )
	{
		isr_t handler = interrupt_handlers[regs.int_no];
		handler(regs);
	}
}
