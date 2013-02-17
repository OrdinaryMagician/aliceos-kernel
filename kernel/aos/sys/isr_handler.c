/*
	isr_handler.c : Common interrupt service routine handler.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/types.h>
#include <hcf.h>

/* the handler itself */
void isr_handler( regs_t regs )
{
	switch ( regs.intno )
	{
	case 0: /* Division by zero exception */
		OHSHI(HCF_DIVZERO, regs);
		break;
	case 1: /* Debug exception */
		OHSHI(HCF_DEBUG, regs);
		break;
	case 2: /* Non maskable interrupt */
		OHSHI(HCF_NMI, regs);
		break;
	case 3: /* Breakpoint exception */
		OHSHI(HCF_BREAK, regs);
		break;
	case 4: /* Overflow exception */
		OHSHI(HCF_OVERFLOW, regs);
		break;
	case 5: /* Out of bounds exception */
		OHSHI(HCF_OOB, regs);
		break;
	case 6: /* Invalid opcode exception */
		/* ignored, seems to happen constantly for no reason */
		break;
	case 7: /* No coprocessor exception */
		OHSHI(HCF_NOCPROC, regs);
		break;
	case 8: /* Double fault */
		OHSHI(HCF_DOUBLEF, regs);
		break;
	case 9: /* Coprocessor segment overrun */
		OHSHI(HCF_CPROCSOVR, regs);
		break;
	case 10: /* Bad TSS */
		OHSHI(HCF_BADTSS, regs);
		break;
	case 11: /* Segment not present */
		OHSHI(HCF_SGNOTP, regs);
		break;
	case 12: /* Stack fault */
		OHSHI(HCF_STACKF, regs);
		break;
	case 13: /* General protection fault */
		OHSHI(HCF_GPF, regs);
		break;
	case 14: /* Page fault */
		OHSHI(HCF_PGF, regs);
		break;
	case 15: /* Unknown interrupt exception */
		OHSHI(HCF_UNKNOWN, regs);
		break;
	case 16: /* Coprocessor fault */
		OHSHI(HCF_CPROCF, regs);
		break;
	case 17: /* Alignment check exception */
		OHSHI(HCF_ALGNEX, regs);
		break;
	case 18: /* Machine check exception */
		OHSHI(HCF_MCHKEX, regs);
		break;
	case 19: /* Reserved */
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
		OHSHI(HCF_RESV, regs);
		break;
	default: /* Unhandled */
		OHSHI(HCF_UNHANDLEDINT, regs);
		break;
	}
}
