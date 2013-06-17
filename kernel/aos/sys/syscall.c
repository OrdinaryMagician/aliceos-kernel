/*
	syscall.c : AOS syscall C code.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/regs.h>
#include <sys/task.h>
#include <printk.h>
/* from task.c */
extern task_t *ctask;
/* base syscall handler */
uint32_t syscall( regs_t *regs )
{
	/* level check */
	switch( (regs->eax&0xC0)>>6 )
	{
	case SCL_1:
		if ( ctask->level < 1 )
			return UINT32_MAX;
		break;
	case SCL_2:
		if ( ctask->level < 2 )
			return UINT32_MAX;
		break;
	}
	switch( regs->eax&0xFF )
	{
	/* level 0 (0x00-0x3F) */
	case SC_NUL0:
		return 0;
	case SC_EXIT:
		return sc_exit(regs);
	case SC_FORK:
		return sc_fork(regs);
	/* level 1 (0x40-0x7F) */
	case SC_NUL1:
		return 0;
	/* level 2 (0x80-0xBF) */
	case SC_NUL2:
		return 0;
	/* level N (0xC0-0xFF) */
	case SC_NULN:
		return 0;
	}
	/* just error out */
	printk("\033[1;31mPID %u performed unknown syscall\033[0m\n",getpid());
	printk("\033[1;31mCALL %#08x\033[0m\n",regs->eax);
	printk("\033[1;31mARG0 %#08x\033[0m\n",regs->ebx);
	printk("\033[1;31mARG1 %#08x\033[0m\n",regs->ecx);
	printk("\033[1;31mARG2 %#08x\033[0m\n",regs->edx);
	return UINT32_MAX;
}
