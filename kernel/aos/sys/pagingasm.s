; pagingasm.s : Assembly functions for paging.
; (C)2012-2013 Marisa Kirisame, UnSX Team.
; Part of AliceOS, the Alice Operating System.
; Released under the MIT License.
[BITS 32]
[ALIGN 4]
[GLOBAL loadcr3]
loadcr3:
	mov eax, [esp+4]
	mov cr3, eax
	ret
[GLOBAL enablepaging]
enablepaging:
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	ret
[GLOBAL getfaultaddr]
getfaultaddr:
	mov eax, cr2
	ret
[GLOBAL pagingenabled]
pagingenabled:
	mov eax, cr0
	shr eax, 31
	and eax, 1
	ret
