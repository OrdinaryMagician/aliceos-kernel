/*
	irq_handler.c : Common interrupt request handler.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/irq_handlers.h>
#include <sys/pic.h>
#include <sys/types.h>

irq_handler_t irq_handlers[256];

/* register an IRQ handler function */
void register_irq_handler( Uint8 n, irq_handler_t handler )
{
	irq_handlers[n] = handler;
}

/* the common handler (which will call any registered handlers) */
void irq_handler( regs_t regs )
{
	pic_eoi(regs.intno-32); /* must take into account the offset */
	if ( irq_handlers[regs.intno] )
		irq_handlers[regs.intno](regs);
}
