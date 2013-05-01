; idt.s : ASM function to write the IDT pointer.
; (C)2012-2013 Marisa Kirisame, UnSX Team.
; Part of AliceOS, the Alice Operating System.
; Released under the MIT License.
BITS 32
ALIGN 4
GLOBAL idt_flush
idt_flush:
	; get parms
	mov eax, [esp+4]
	; load idt
	lidt [eax]
	; done
	ret
