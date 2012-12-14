; boot.s : Multiboot-compatible ASM start point for the kernel.
; (C)2012 Marisa Kirisame, UnSX Team.
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

[BITS 32] ; we're going to target i686, anyway

; putting all them datas in the kernel img
[GLOBAL mboot]
[EXTERN code]
[EXTERN bss]
[EXTERN end]
mboot:
	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_HEADER_FLAGS
	dd MULTIBOOT_CHECKSUM
	dd mboot
	dd code
	dd bss
	dd end
	dd start

; getting started~
[GLOBAL start]
[EXTERN kmain]
start:
	; muh stack pointer
	mov esp, 0x7FFFF
	push esp
	; dem headers
	push eax
	push ebx
	; no interrupts pls
	cli
	; ... and we're done with asm for now
	call kmain
	hlt

; BEES
SECTION .bss
	resb 8192 ; reservan 8KiB of memory
