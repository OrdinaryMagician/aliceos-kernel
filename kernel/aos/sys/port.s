; port.s : I/O port functions.
; (C)2012-2013 Marisa Kirisame, UnSX Team.
; Part of AliceOS, the Alice Operating System.
; Released under the MIT License.
[BITS 32]
[ALIGN 4]

; read/write bytes
[GLOBAL inport_b]
inport_b:
	push ebp
	mov ebp, esp
	mov edx, [ebp+8]
	xor eax, eax
	in al, dx
	pop ebp
	ret
[GLOBAL outport_b]
outport_b:
	push ebp
	mov ebp, esp
	mov edx, [ebp+8]
	mov eax, [ebp+12]
	out dx, al
	pop ebp
	ret

; read/write words
[GLOBAL inport_w]
inport_w:
	push ebp
	mov ebp, esp
	mov edx, [ebp+8]
	xor eax, eax
	in ax, dx
	pop ebp
	ret
[GLOBAL outport_w]
outport_w:
	push ebp
	mov ebp, esp
	mov edx, [ebp+8]
	mov eax, [ebp+12]
	out dx, ax
	pop ebp
	ret

; read/write "longs"
[GLOBAL inport_l]
inport_l:
	push ebp
	mov ebp, esp
	mov edx, [ebp+8]
	xor eax, eax
	in eax, dx
	pop ebp
	ret
[GLOBAL outport_l]
outport_l:
	push ebp
	mov ebp, esp
	mov edx, [ebp+8]
	mov eax, [ebp+12]
	out dx, eax
	pop ebp
	ret
