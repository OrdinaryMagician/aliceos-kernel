/*
	isr_handler.c : Common interrupt service routine handler.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/types.h>
#include <sys/regs.h>
#include <sys/isr_handlers.h>
#include <hcf.h>
#include <memops.h>
#include <sys/helpers.h>
#include <sys/syscall.h>
/* handler listing */
static isr_handler_t isr_handlers[32];
/* register an ISR handler function */
void register_isr_handler( uint8_t n, isr_handler_t handler )
{
	isr_handlers[n] = handler;
}
/* clear ISR handlers */
void isr_clearhandlers( void )
{
	memset(&isr_handlers[0],0,sizeof(isr_handler_t)*32);
}
/* the handler itself */
void isr_handler( regs_t *regs )
{
	uint32_t ret = regs->eax;
	int_disable();
	/* call a specific handler if available */
	if ( (regs->intno < 32) && isr_handlers[regs->intno] )
	{
		isr_handler_t hnd = isr_handlers[regs->intno];
		hnd(regs);
		int_enable();
		return;
	}
	/* default handlers, mostly just calls to halt and catch fire */
	switch ( regs->intno )
	{
	case 0: /* Division by zero exception */
		OHSHI(HCF_DIVZERO,regs);
		break;
	case 1: /* Debug exception */
		OHSHI(HCF_DEBUG,regs);
		break;
	case 2: /* Non maskable interrupt */
		OHSHI(HCF_NMI,regs);
		break;
	case 3: /* Breakpoint exception */
		OHSHI(HCF_BREAK,regs);
		break;
	case 4: /* Overflow exception */
		OHSHI(HCF_OVERFLOW,regs);
		break;
	case 5: /* Out of bounds exception */
		OHSHI(HCF_OOB,regs);
		break;
	case 6: /* Invalid opcode exception */
		OHSHI(HCF_INVOPCODE,regs);
		break;
	case 7: /* No coprocessor exception */
		OHSHI(HCF_NOCPROC,regs);
		break;
	case 8: /* Double fault */
		OHSHI(HCF_DOUBLEF,regs);
		break;
	case 9: /* Coprocessor segment overrun */
		OHSHI(HCF_CPROCSOVR,regs);
		break;
	case 10: /* Bad TSS */
		OHSHI(HCF_BADTSS,regs);
		break;
	case 11: /* Segment not present */
		OHSHI(HCF_SGNOTP,regs);
		break;
	case 12: /* Stack fault */
		OHSHI(HCF_STACKF,regs);
		break;
	case 13: /* General protection fault */
		OHSHI(HCF_GPF,regs);
		break;
	case 14: /* Page fault */
		OHSHI(HCF_PGF,regs);
		break;
	case 15: /* Unknown interrupt exception */
		OHSHI(HCF_UNKNOWN,regs);
		break;
	case 16: /* Coprocessor fault */
		OHSHI(HCF_CPROCF,regs);
		break;
	case 17: /* Alignment check exception */
		OHSHI(HCF_ALGNEX,regs);
		break;
	case 18: /* Machine check exception */
		OHSHI(HCF_MCHKEX,regs);
		break;
	/* Reserved */
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
	case 24:
	case 25:
	case 26:
	case 27:
	case 28:
	case 29:
	case 30:
	case 31:
		OHSHI(HCF_RESV,regs);
		break;
	/* syscall */
	case 128:
		ret = syscall(regs);
		break;
	default: /* Unhandled */
		OHSHI(HCF_UNHANDLEDINT,regs);
		break;
	}
	regs->eax = ret;
	int_enable();
}
