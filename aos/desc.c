/*
	desc.c : Descriptor tables.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/

#include "desc.h"
#include "isr.h"
#include "screen.h"
#include "string.h"
#include "port.h"

extern void gdt_flush( Uint32 );
extern void idt_flush( Uint32 );

static void init_gdt( void );
static void gdt_set_gate( Sint32, Uint32, Uint32, Uint8, Uint8 );
static void init_idt( void );
static void idt_set_gate( Uint8, Uint32, Uint16, Uint8 );

gdt_entry_t gdt_entries[5];
gdt_ptr_t gdt_ptr;
idt_entry_t idt_entries[256];
idt_ptr_t idt_ptr;

extern isr_t interrupt_handlers[];

void init_descriptor_tables( void )
{
	init_gdt();
	init_idt();
	memset(&interrupt_handlers,0,sizeof(isr_t)*256);
}

static void init_gdt( void )
{
	gdt_ptr.limit = sizeof(gdt_entry_t)*5-1;
	gdt_ptr.base = (Uint32)&gdt_entries;
	gdt_set_gate(0,0,0,0,0); /* Null */
	gdt_set_gate(1,0,0xFFFFFFFF,0x9A,0xCF); /* Code */
	gdt_set_gate(2,0,0xFFFFFFFF,0x92,0XCF); /* Data */
	gdt_set_gate(3,0,0xFFFFFFFF,0xFA,0xCF); /* Usermode code */
	gdt_set_gate(4,0,0xFFFFFFFF,0xF2,0XCF); /* Usermode data */
	gdt_flush((Uint32)&gdt_ptr);
}

static void init_idt( void )
{
	idt_ptr.limit = sizeof(idt_entry_t)*256-1;
	idt_ptr.base = (Uint32)&idt_entries;
	memset(&idt_entries,0,sizeof(idt_entry_t)*256);
	outb(0x20,0x11);
	outb(0xA0,0x11);
	outb(0x21,0x20);
	outb(0xA1,0x28);
	outb(0x21,0x04);
	outb(0xA1,0x02);
	outb(0x21,0x01);
	outb(0xA1,0x01);
	outb(0x21,0x00);
	outb(0xA1,0x00);
	idt_set_gate(0,(Uint32)isr0,0x08,0x8E);
	idt_set_gate(1,(Uint32)isr1,0x08,0x8E);
	idt_set_gate(2,(Uint32)isr2,0x08,0x8E);
	idt_set_gate(3,(Uint32)isr3,0x08,0x8E);
	idt_set_gate(4,(Uint32)isr4,0x08,0x8E);
	idt_set_gate(5,(Uint32)isr5,0x08,0x8E);
	idt_set_gate(6,(Uint32)isr6,0x08,0x8E);
	idt_set_gate(7,(Uint32)isr7,0x08,0x8E);
	idt_set_gate(8,(Uint32)isr8,0x08,0x8E);
	idt_set_gate(9,(Uint32)isr9,0x08,0x8E);
	idt_set_gate(10,(Uint32)isr10,0x08,0x8E);
	idt_set_gate(11,(Uint32)isr11,0x08,0x8E);
	idt_set_gate(12,(Uint32)isr12,0x08,0x8E);
	idt_set_gate(13,(Uint32)isr13,0x08,0x8E);
	idt_set_gate(14,(Uint32)isr14,0x08,0x8E);
	idt_set_gate(15,(Uint32)isr15,0x08,0x8E);
	idt_set_gate(16,(Uint32)isr16,0x08,0x8E);
	idt_set_gate(17,(Uint32)isr17,0x08,0x8E);
	idt_set_gate(18,(Uint32)isr18,0x08,0x8E);
	idt_set_gate(19,(Uint32)isr19,0x08,0x8E);
	idt_set_gate(20,(Uint32)isr20,0x08,0x8E);
	idt_set_gate(21,(Uint32)isr21,0x08,0x8E);
	idt_set_gate(22,(Uint32)isr22,0x08,0x8E);
	idt_set_gate(23,(Uint32)isr23,0x08,0x8E);
	idt_set_gate(24,(Uint32)isr24,0x08,0x8E);
	idt_set_gate(25,(Uint32)isr25,0x08,0x8E);
	idt_set_gate(26,(Uint32)isr26,0x08,0x8E);
	idt_set_gate(27,(Uint32)isr27,0x08,0x8E);
	idt_set_gate(28,(Uint32)isr28,0x08,0x8E);
	idt_set_gate(29,(Uint32)isr29,0x08,0x8E);
	idt_set_gate(30,(Uint32)isr30,0x08,0x8E);
	idt_set_gate(31,(Uint32)isr31,0x08,0x8E);
	idt_set_gate(32,(Uint32)irq0,0x08,0x8E);
	idt_set_gate(33,(Uint32)irq1,0x08,0x8E);
	idt_set_gate(34,(Uint32)irq2,0x08,0x8E);
	idt_set_gate(35,(Uint32)irq3,0x08,0x8E);
	idt_set_gate(36,(Uint32)irq4,0x08,0x8E);
	idt_set_gate(37,(Uint32)irq5,0x08,0x8E);
	idt_set_gate(38,(Uint32)irq6,0x08,0x8E);
	idt_set_gate(39,(Uint32)irq7,0x08,0x8E);
	idt_set_gate(40,(Uint32)irq8,0x08,0x8E);
	idt_set_gate(41,(Uint32)irq9,0x08,0x8E);
	idt_set_gate(42,(Uint32)irq10,0x08,0x8E);
	idt_set_gate(43,(Uint32)irq11,0x08,0x8E);
	idt_set_gate(44,(Uint32)irq12,0x08,0x8E);
	idt_set_gate(45,(Uint32)irq13,0x08,0x8E);
	idt_set_gate(46,(Uint32)irq14,0x08,0x8E);
	idt_set_gate(47,(Uint32)irq15,0x08,0x8E);
	idt_flush((Uint32)&idt_ptr);
}

static void gdt_set_gate( Sint32 num, Uint32 base, Uint32 limit, Uint8 access, Uint8 gran )
{
	gdt_entries[num].base_low = base&0xFFFF;
	gdt_entries[num].base_middle = (base>>16)&0xFF;
	gdt_entries[num].base_high = (base>>24)&0xFF;
	gdt_entries[num].limit_low = limit&0xFFFF;
	gdt_entries[num].granularity = (limit>>16)&0x0F;
	gdt_entries[num].granularity |= gran&0xF0;
	gdt_entries[num].access = access;
}

static void idt_set_gate( Uint8 num, Uint32 base, Uint16 sel, Uint8 flags )
{
	idt_entries[num].base_lo = base&0xFFFF;
	idt_entries[num].base_hi = (base>>16)&0xFFFF;
	idt_entries[num].sel = sel;
	idt_entries[num].always0 = 0;
	idt_entries[num].flags = flags /* | 0x60 */;
}
