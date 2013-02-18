/*
	isr_handlers.h : Interface for defining ISR handlers.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef ISR_HANDLERS_H
#define ISR_HANDLERS_H
#include <sys/types.h>
#include <sys/regs.h>

/* just a harmless typedef */
typedef void (*isr_handler_t)( regs_t *regs );
/* register an ISR handler function */
void register_isr_handler( Uint8 n, isr_handler_t handler );
/* clear ISR handlers */
void irsr_clearhandlers( void );

#endif
