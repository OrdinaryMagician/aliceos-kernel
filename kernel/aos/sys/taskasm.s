; taskasm.s : Multitasking.
; (C)2012-2013 Marisa Kirisame, UnSX Team.
; Part of AliceOS, the Alice Operating System.
; Released under the MIT License.
[GLOBAL flush_tlb]
flush_tlb:
	; read from cr3, write to cr3
	; that's enough to flush it
	mov eax, cr3
	mov cr3, eax
	ret
[GLOBAL read_eip]
read_eip:
	pop eax
	jmp eax
