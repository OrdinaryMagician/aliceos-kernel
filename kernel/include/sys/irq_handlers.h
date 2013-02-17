/*
	irq_handlers.h : Interface for defining IRQ handlers.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef IRQ_HANDLERS_H
#define IRQ_HANDLERS_H
#include <sys/types.h>

typedef struct
{
	Uint32 ds;
	Uint32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
	Uint32 intno, errno;
	Uint32 eip, cs, eflags, uesp, ss;
} regs_t;

/* defines for IRQs */
#define IRQ0  32
#define IRQ1  33
#define IRQ2  34
#define IRQ3  35
#define IRQ4  36
#define IRQ5  37
#define IRQ6  38
#define IRQ7  39
#define IRQ8  40
#define IRQ9  41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

/* just a harmless typedef */
typedef void (*irq_handler_t)( regs_t regs );
/* register an IRQ handler function */
void register_irq_handler( Uint8 n, irq_handler_t handler );
/* clear IRQ handlers */
void irq_clearhandlers( void );

#endif
