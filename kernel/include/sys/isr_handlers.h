/*
	isr_handlers.h : Interface for defining ISR handlers.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
/* just a harmless typedef */
typedef void (*isr_handler_t)( regs_t *regs );
/* register an ISR handler function */
void register_isr_handler( uint8_t n, isr_handler_t handler );
/* clear ISR handlers */
void isr_clearhandlers( void );
