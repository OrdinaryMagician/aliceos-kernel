; helpers.s : Short helper functions and macros.
; (C)2012-2013 Marisa Kirisame, UnSX Team.
; Part of AliceOS, the Alice Operating System.
; Released under the MIT License.
[BITS 32]
[ALIGN 4]
[GLOBAL int_enable]
int_enable:
	sti
	ret
[GLOBAL int_disable]
int_disable:
	cli
	ret
[GLOBAL khalt]
khalt:
	mov eax, [esp+4]
	jmp $
	ret
