; bootx86.s : Multiboot-compatible ASM start point for the kernel.
; (C)2012-2013 Marisa Kirisame, UnSX Team.
; Part of AliceOS, the Alice Operating System.
; Released under the MIT License.
ALIGN 4
BITS 32 ; we're going to target i386, anyway
; Multiboot flags
MULTIBOOT_PAGE_ALIGN	equ 1<<0 ; page alignment pls
MULTIBOOT_MEMORY_INFO	equ 1<<1 ; mem info too
MULTIBOOT_HEADER_FLAGS	equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
; Dat magic
MULTIBOOT_HEADER_MAGIC	equ 0x1BADB002
; Ketchu- checksum, I mean
MULTIBOOT_CHECKSUM	equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
; these things are defined in the linker script
EXTERN code,data,bss,end
; don't mind me, I got bored
GLOBAL signature
signature:
	; 64-byte 2-part kernel signature
	; UTF-8 string, should be located 4096 bytes into the file
	; an easy way to read it: strings -eS vmaliceos | head -n 2
	db 0x41,0x4C,0x49,0x43,0x45,0x4F,0x53,0xE3
	db 0x81,0xA7,0xE3,0x82,0x8C,0x41,0x4F,0x53
	db 0x4B,0x45,0x52,0x4E,0x45,0x4C,0x00,0x00
	db 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	db 0x28,0x43,0x29,0x32,0x30,0x31,0x32,0x2D
	db 0x32,0x30,0x31,0x33,0x20,0xE9,0x9C,0xA7
	db 0xE9,0x9B,0xA8,0x20,0xE9,0xAD,0x94,0xE7
	db 0x90,0x86,0xE6,0xB2,0x99,0x00,0x00,0x00
; putting all them datas in the kernel img
GLOBAL mboot
mboot:
	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_HEADER_FLAGS
	dd MULTIBOOT_CHECKSUM
; getting started~
GLOBAL start
EXTERN kmain
start:
	; no interrupts pls
	cli
	; move stack
	mov esp, stack_beg
	; muh stack pointer
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
GLOBAL stack_end,stack_beg
stack_end:
	resb 8192 ; reservan 8KiB of memory
stack_beg: