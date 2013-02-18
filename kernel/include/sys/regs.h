/*
	regs.h : regs_t struct used by interrupt handlers.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef REGS_H
#define REGS_H
#include <sys/types.h>
typedef struct
{
	Uint32 gs, fs, es, ds;
	Uint32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
	Uint32 intno, errno;
	Uint32 eip, cs, eflags, uesp, ss;
} regs_t;
#endif
