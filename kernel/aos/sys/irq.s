; irq.s : Interrupt requests.
; (C)2012-2013 Marisa Kirisame, UnSX Team.
; Part of AliceOS, the Alice Operating System.
; Released under the MIT License.
[BITS 32]
; oh macros~
%macro IRQ 2
[GLOBAL irq%1]
irq%1:
	cli
	push byte 0
	push byte %2
	jmp irq_common_stub
%endmacro
; the interrupts
IRQ 0,32
IRQ 1,33
IRQ 2,34
IRQ 3,35
IRQ 4,36
IRQ 5,37
IRQ 6,38
IRQ 7,39
IRQ 8,40
IRQ 9,41
IRQ 10,42
IRQ 11,43
IRQ 12,44
IRQ 13,45
IRQ 14,46
IRQ 15,47
[EXTERN irq_handler]
irq_common_stub:
	pusha ; push all the general purpose registers
	; save data segment descriptor
	mov ax, ds
	push eax
	; load kernel data segment descriptor
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	; call the C function
	call irq_handler
	; reload original data segment descriptor
	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	; pop registers back
	popa
	; clean up code, enable interrupts
	add esp, 8
	sti
	; pop CS, EIP, EFLAGS, SS and ESP
	iret
