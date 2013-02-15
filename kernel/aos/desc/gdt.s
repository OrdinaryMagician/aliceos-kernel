; gdt.s : ASM function to write the GDT pointer.
; (C)2012-2013 Marisa Kirisame, UnSX Team.
; Part of AliceOS, the Alice Operating System.
; Released under the MIT License.
[BITS 32]
[GLOBAL gdt_flush]
gdt_flush:
	; get parms
	mov eax, [esp+4]
	; load gdt
	lgdt [eax]
	; load up data segments onto it
	mov ax, 0x10 ; data segment is 16 bytes in
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	; back to code segment...
	jmp 0x08:.flush
.flush:
	; ...and done!
	ret
