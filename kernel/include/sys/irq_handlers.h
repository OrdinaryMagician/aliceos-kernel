/*
	irq_handlers.h : Interface for defining IRQ handlers.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef IRQ_HANDLERS_H
#define IRQ_HANDLERS_H
#include <sys/types.h>
#include <sys/regs.h>
/* just a harmless typedef */
typedef void (*irq_handler_t)( regs_t *regs );
/* send end-of-interrupt to PIC */
void irq_eoi( uint8_t irq );
/* register an IRQ handler function */
void register_irq_handler( uint8_t n, irq_handler_t handler );
/* clear IRQ handlers */
void irq_clearhandlers( void );
#endif
