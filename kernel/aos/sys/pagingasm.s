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
[GLOBAL copy_page_physical]
copy_page_physical:
	; must preserve ebx (__cdecl or some stuff, idk)
	push ebx
	; push EFLAGS
	pushf
	cli
	; addresses
	mov ebx, [esp+12]
	mov ecx, [esp+16]
	; disable paging
	mov edx, cr0
	and edx, 0x7fffffff
	mov cr0, edx
	; 1024 words to copy (4KiB)
	mov edx, 0x400
.loop:
	; copy a word
	mov eax, [ebx]
	mov [ecx], eax
	; advance
	add ebx, 4
	add ecx, 4
	dec edx
	jnz .loop
	; enable paging
	mov edx, cr0
	or edx, 0x80000000
	mov cr0, edx
	; pop EFLAGS
	popf
	; recover ebx
	pop ebx
	; endo
	ret
