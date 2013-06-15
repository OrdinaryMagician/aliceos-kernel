; syscallasm.s : Assembly section of syscall handling.
; (C)2012-2013 Marisa Kirisame, UnSX Team.
; Part of AliceOS, the Alice Operating System.
; Released under the MIT License.
BITS 32
ALIGN 4
; perform a syscall
GLOBAL sc
sc:
	push ebp
	mov ebp, esp
	mov eax, [ebp+8]
	mov ebx, [ebp+12]
	mov ecx, [ebp+16]
	mov edx, [ebp+20]
	int 80h
	pop ebp
	ret
