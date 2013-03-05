/*
	irq_handler.c : Common interrupt request handler.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/irq_handlers.h>
#include <sys/regs.h>
#include <sys/port.h>
#include <sys/types.h>
#include <sys/serial.h>
#include <sys/helpers.h>
#include <printk.h>
#include <memops.h>

static irq_handler_t irq_handlers[16];

/* register an IRQ handler function */
void register_irq_handler( Uint8 n, irq_handler_t handler )
{
	irq_handlers[n] = handler;
}

/* the common handler (which will call any registered handlers) */
void irq_handler( regs_t *regs )
{
	int_disable();
	Uint8 irq = regs->intno-32;
	/* call a specific handler if available */
	if ( irq_handlers[irq] )
	{
		irq_handler_t hnd = irq_handlers[irq];
		hnd(regs);
	}
	/* send end-of-interrupt to PIC */
	if ( irq >= 12 )
		outport_b(0xA0,0x20);
	outport_b(0x20,0x20);
	int_enable();
}

/* clear IRQ handlers */
void irq_clearhandlers( void )
{
	memset((Uint8*)&irq_handlers[0],0,sizeof(irq_handler_t)*16);
}
