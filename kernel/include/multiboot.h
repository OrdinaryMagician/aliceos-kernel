/*
	multiboot.h : Stuff for multiboot.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef MULTIBOOT_H
#define MULTIBOOT_H
#include <types.h>
#include <helpers.h>

#define MULTIBOOT_FLAG_MEM     0x001
#define MULTIBOOT_FLAG_DEVICE  0x002
#define MULTIBOOT_FLAG_CMDLINE 0x004
#define MULTIBOOT_FLAG_MODS    0x008
#define MULTIBOOT_FLAG_AOUT    0x010
#define MULTIBOOT_FLAG_ELF     0x020
#define MULTIBOOT_FLAG_MMAP    0x040
#define MULTIBOOT_FLAG_CONFIG  0x080
#define MULTIBOOT_FLAG_LOADER  0x100
#define MULTIBOOT_FLAG_APM     0x200
#define MULTIBOOT_FLAG_VBE     0x400

struct multiboot
{
	Uint32 flags;
	Uint32 mem_lower;
	Uint32 mem_upper;
	Uint32 boot_device;
	Uint32 cmdline;
	Uint32 mods_count;
	Uint32 mods_addr;
	Uint32 num;
	Uint32 size;
	Uint32 addr;
	Uint32 shndx;
	Uint32 mmap_length;
	Uint32 mmap_addr;
	Uint32 drives_length;
	Uint32 drives_addr;
	Uint32 config_table;
	Uint32 boot_loader_name;
	Uint32 apm_table;
	Uint32 vbe_control_info;
	Uint32 vbe_mode_info;
	Uint32 vbe_mode;
	Uint32 vbe_interface_seg;
	Uint32 vbe_interface_off;
	Uint32 vbe_interface_len;
} attribute ((packed));
typedef struct multiboot_header multiboot_header_t;

#endif
