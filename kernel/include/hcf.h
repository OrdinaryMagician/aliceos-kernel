/*
	hcf.h : Halt and catch fire, also known as OHSHI().
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef HCF_H
#define HCF_H
#include <sys/types.h>
typedef struct
{
	Uint32 ds;
	Uint32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
	Uint32 intno, errno;
	Uint32 eip, cs, eflags, uesp, ss;
} regs_t;

/* predefined messages */
#define HCF_UNHANDLEDINT "Unhandled Interrupt"
#define HCF_DIVZERO "Division by Zero"
#define HCF_DEBUG "Debug"
#define HCF_NMI "Non Maskable Interrupt"
#define HCF_BREAK "Breakpoint"
#define HCF_OVERFLOW "Overflow"
#define HCF_OOB "Out Of Bounds"
#define HCF_INVOPCODE "Invalid Opcode"
#define HCF_NOCPROC "No Coprocessor"
#define HCF_DOUBLEF "Double Fault"
#define HCF_CPROCSOVR "Coprocessor Segment Overrun"
#define HCF_BADTSS "Bad TSS"
#define HCF_SGNOTP "Segment Not Present"
#define HCF_STACKF "Stack Fault"
#define HCF_GPF "General Protection Fault"
#define HCF_PGF "Page Fault"
#define HCF_UNKNOWN "Unknown Interrupt"
#define HCF_CPROCF "Coprocessor Fault"
#define HCF_ALGNEX "Alignment Check"
#define HCF_MCHKEX "Machine Check"
#define HCF_RESV "Reserved Interrupt"

/* for when you screwed up pretty bad */
void OHSHI( char *mesg, regs_t regs );
#endif
