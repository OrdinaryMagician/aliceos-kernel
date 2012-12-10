#ifndef ISR_H
#define ISR_H
#include "types.h"
#include "screen.h"
struct registers
{
	Uint32 ds;
	Uint32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
	Uint32 int_no, err_code;
	Uint32 eip, cs, eflags, useresp, ss;
};
typedef struct registers registers_t;
#endif
