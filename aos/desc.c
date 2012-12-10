/*
	desc.c : Descriptor tables.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/

#include "desc.h"

extern void gdt_flush( Uint32 );

static void init_gdt( void );
static void gdt_set_gate( Sint32, Uint32, Uint32, Uint8, Uint8 );

gdt_entry_t gdt_entries[5];
gdt_ptr_t gdt_ptr;
idt_entry_t idt_entries[256];
idt_ptr_t idt_ptr;

void init_descriptor_tables( void )
{
	init_gdt();
}

static void init_gdt( void )
{
	gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
	gdt_ptr.base = (Uint32)&gdt_entries;
	gdt_set_gate(0,0,0,0,0); /* Null */
	gdt_set_gate(1,0,0xFFFFFFFF,0x9A,0xCF); /* Code */
	gdt_set_gate(2,0,0xFFFFFFFF,0x92,0XCF); /* Data */
	gdt_set_gate(3,0,0xFFFFFFFF,0xFA,0xCF); /* Usermode code */
	gdt_set_gate(4,0,0xFFFFFFFF;0xF2,0XCF); /* Usermode data */
	gdt_flush((Uint32)&gdt_ptr);
}

static void gdt_set_gate( Sint32 num, Uint32 base, Uint32 limit, Uint8 access, Uint8 gran )
{
	gdt_entries[num].base_low = base&0xFFFF;
	gdt_entries[num].base_middle (base>>16)&0xFF
	gdt_entries[num].base_high = (base>>24)&0xFF;
	gdt_entries[num].limit_low = limit&0xFFFF;
	gdt_entries[num].granularity = (limit>>16)&0x0F;
	gdt_entries[num].granularity |= gran & 0xF0;
	gdt_entries[num].access = access;
}
