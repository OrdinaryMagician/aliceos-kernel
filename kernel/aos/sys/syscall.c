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
/* base syscall handler */
uint32_t syscall( regs_t *regs )
{
	switch( regs->eax&0xFF )
	{
	/* level 0 (0x00-0x3F) */
	/* level 1 (0x40-0x7F) */
	/* level 2 (0x80-0xBF) */
	/* level N (0xC0-0xFF) */
	default:
		/* nuthin' */
		break;
	}
	/* just error out */
	printk("\033[1;31mPID %u performed unknown syscall\033[0m\n",getpid());
	printk("\033[1;31mCALL %#08x\033[0m\n",regs->eax);
	printk("\033[1;31mARG0 %#08x\033[0m\n",regs->ebx);
	printk("\033[1;31mARG1 %#08x\033[0m\n",regs->ecx);
	printk("\033[1;31mARG2 %#08x\033[0m\n",regs->edx);
	return UINT32_MAX;
}
