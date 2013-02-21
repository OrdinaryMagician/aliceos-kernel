/*
	multiboot.h : Stuff for multiboot.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef MULTIBOOT_H
#define MULTIBOOT_H
#include <sys/types.h>
#include <sys/helpers.h>

#define MULTIBOOT_MAGIC        0x1BADB002
#define MULTIBOOT_EAX_MAGIC    0x2BADB002
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

typedef struct
{
	Uint32 size;
	Uint32 addr_l;
	Uint32 addr_h;
	Uint32 len_l;
	Uint32 len_h;
	Uint32 type;
} attribute((packed)) mmap_entry_t;

typedef struct
{
	Uint32 mod_start;
	Uint32 mod_end;
	Uint32 cmdline;
	Uint32 pad;
} attribute((packed)) mbootmod_t;

typedef struct
{
	Uint32 tabsize;
	Uint32 strsize;
	Uint32 addr;
	Uint32 reserved;
} attribute((packed)) aout_syms_t;

typedef struct
{
	Uint32 num;
	Uint32 size;
	Uint32 addr;
	Uint32 shndx;
} attribute((packed)) elf_hdr_t;

typedef struct
{
	Uint32 size;
	Uint8 drive_number;
	Uint8 drive_mode;
	Uint16 drive_cylinders;
	Uint8 drive_heads;
	Uint8 drive_sectors;
	Uint8 *drive_ports;
} attribute((packed)) drive_t;

typedef struct
{
	Uint16 version;
	Uint16 cseg;
	Uint32 offset;
	Uint16 cseg_16;
	Uint16 dseg;
	Uint16 flags;
	Uint16 cseg_len;
	Uint16 cseg_16_len;
	Uint16 dseg_len;
} attribute((packed)) apm_table_t;

typedef struct
{
	Uint16 attributes;
	Uint8 winA,winB;
	Uint16 granularity;
	Uint16 winsize;
	Uint16 segmentA, segmentB;
	Uint32 realFctPtr;
	Uint16 pitch;
	Uint16 Xres, Yres;
	Uint8 Wchar, Ychar, planes, bpp, banks;
	Uint8 memory_model, bank_size, image_pages;
	Uint8 reserved0;
	Uint8 red_mask, red_position;
	Uint8 green_mask, green_position;
	Uint8 blue_mask, blue_position;
	Uint8 rsv_mask, rsv_position;
	Uint8 directcolor_attributes;
	Uint32 physbase;
	Uint32 reserved1;
	Uint16 reserved2;
} attribute((packed)) vbe_info_t;

typedef struct
{
	Uint32 flags;
	Uint32 mem_lower;
	Uint32 mem_upper;
	Uint32 boot_device;
	Uint32 cmdline;
	Uint32 mods_count;
	Uint32 mods_addr;
	union
	{
		aout_syms_t aout;
		elf_hdr_t elf;
	} syms;
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
} attribute((packed)) multiboot_t;

#endif
