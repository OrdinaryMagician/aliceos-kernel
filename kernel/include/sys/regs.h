/*
	regs.h : regs_t struct used by interrupt handlers.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
typedef struct
{
	uint32_t gs, fs, es, ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t intno, errno;
	uint32_t eip, cs, eflags, uesp, ss;
} regs_t;
