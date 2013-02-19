; boot.s : Multiboot-compatible ASM start point for the kernel.
; (C)2012-2013 Marisa Kirisame, UnSX Team.
; Part of AliceOS, the Alice Operating System.
; Released under the MIT License.

; Multiboot flags
MULTIBOOT_PAGE_ALIGN	equ 1<<0 ; page alignment pls
MULTIBOOT_MEMORY_INFO	equ 1<<1 ; mem info too
MULTIBOOT_HEADER_FLAGS	equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
; Dat magic
MULTIBOOT_HEADER_MAGIC	equ 0x1BADB002
; Ketchu- checksum, I mean
MULTIBOOT_CHECKSUM	equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

[BITS 32] ; we're going to target i386, anyway
[ALIGN 4]

; putting all them datas in the kernel img
[GLOBAL mboot]
[EXTERN code]
[EXTERN bss]
[EXTERN end]
mboot:
	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_HEADER_FLAGS
	dd MULTIBOOT_CHECKSUM

; getting started~
[GLOBAL start]
[EXTERN kmain]
start:
	; no interrupts pls
	cli
	; muh stack pointer
	mov esp, 0x7FFFF
	push esp
	; dem headers
	push eax
	push ebx
	; ... and we're done with asm for now
	call kmain
	; an infinite loop at the end
	jmp $

; BEES
SECTION .bss
	resb 8192 ; reservan 8KiB of memory
