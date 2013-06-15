/*
	syscall.h : AOS syscall C code.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/types.h>
#include <sys/regs.h>
/* base syscall handler */
uint32_t syscall( regs_t *regs );
/* perform a syscall */
uint32_t sc( uint32_t c, uint32_t a0, uint32_t a1, uint32_t a2 );
