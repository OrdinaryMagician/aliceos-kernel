/*
	syscall.h : AOS syscall C code.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/types.h>
#include <sys/regs.h>
/* Syscall levels */
#define SCL_0 0x00	/* Level 0 (unprivileged) syscalls */
#define SCL_1 0x01	/* Level 1 (administrative) syscalls */
#define SCL_2 0x02	/* Level 2 (kernel) syscalls */
#define SCL_N 0x03	/* Level N (nokinan) syscalls */
/* Syscall IDs */
/* Level 0 */
#define SC_NUL0 0x00	/* Null level 0 syscall (debug) */
#define SC_EXIT 0x01	/* End current task and return to parent */
#define SC_FORK 0x02	/* Spawn a child task at current position */
#define SC_MIN0 SC_NUL0	/* Lowest level 0 syscall */
#define SC_MAX0 SC_FORK	/* Highest level 0 syscall */
/* Level 1 */
#define SC_NUL1 0x40	/* Null level 1 syscall (debug) */
#define SC_MIN1 SC_NUL1	/* Lowest level 0 syscall */
#define SC_MAX1 SC_NUL1	/* Highest level 0 syscall */
/* Level 2 */
#define SC_NUL2 0x80	/* Null level 2 syscall (debug) */
#define SC_MIN2 SC_NUL2	/* Lowest level 0 syscall */
#define SC_MAX2 SC_NUL2	/* Highest level 0 syscall */
/* Level N */
#define SC_NULN 0xC0	/* Null level N syscall (debug) */
#define SC_MINN SC_NULN	/* Lowest level 0 syscall */
#define SC_MAXN SC_NULN	/* Highest level 0 syscall */
/* base syscall handler */
uint32_t syscall( regs_t *regs );
/* perform a syscall */
uint32_t sc( uint32_t c, uint32_t a0, uint32_t a1, uint32_t a2 );
