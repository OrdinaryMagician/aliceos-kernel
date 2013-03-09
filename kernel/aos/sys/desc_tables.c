/*
	desc_tables.c : definitions for descriptor tables.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/desc_tables.h>
#include <sys/isr.h>
#include <sys/irq.h>
#include <sys/port.h>
#include <memops.h>
#include <sys/helpers.h>
#include <sys/irq_handlers.h>
/* Write the actual GDT pointer */
extern void gdt_flush( Uint32 ptr );
/* Write the actual IDT pointer */
extern void idt_flush( Uint32 ptr );
/* start up GDT */
static void init_gdt( void );
/* set the values of a GDT entry */
static void gdt_setgate( Sint32 num, Uint32 bas, Uint32 lim, Uint8 acc,
			 Uint8 grn );
/* start up IDT */
static void init_idt( void );
/* set the values of a IDT entry */
static void idt_setgate( Uint8 num, Uint32 bas, Uint16 sel, Uint8 flg );
/* table arrays */
static gdt_entry_t gdt_ents[5];
static gdt_ptr_t gdt_ptr;
static idt_entry_t idt_ents[256];
static idt_ptr_t idt_ptr;
/* Initialize descriptor tables (duh) */
void init_descriptor_tables( void )
{
	/* global descriptor table */
	init_gdt();
	/* interrupt descriptor table */
	init_idt();
	/* now we can start interrupts */
	int_enable();
}
/* start up GDT */
static void init_gdt( void )
{
	gdt_ptr.limit = (sizeof(gdt_entry_t)*5)-1;
	gdt_ptr.base = (Uint32)&gdt_ents;
	/* segments (null, code, data, user mode code, user mode data) */
	gdt_setgate(0,0,0x00000000,0x00,0x00);
	gdt_setgate(1,0,0xFFFFFFFF,0x9A,0xCF);
	gdt_setgate(2,0,0xFFFFFFFF,0x92,0xCF);
	gdt_setgate(3,0,0xFFFFFFFF,0xFA,0xCF);
	gdt_setgate(4,0,0xFFFFFFFF,0xF2,0xCF);
	/* flush~ */
	gdt_flush((Uint32)&gdt_ptr);
}
/* set the values of a GDT entry */
static void gdt_setgate( Sint32 num, Uint32 bas, Uint32 lim, Uint8 acc,
			 Uint8 grn )
{
	gdt_ents[num].base_l = (bas&0xFFFF);
	gdt_ents[num].base_m = (bas>>16)&0xFF;
	gdt_ents[num].base_h = (bas>>24)&0xFF;
	gdt_ents[num].limit_l = (lim&0xFFFF);
	gdt_ents[num].granularity = ((lim>>16)&0x0F)|(grn&0xF0);
	gdt_ents[num].access = acc;
}
/* start up IDT */
static void init_idt( void )
{
	idt_ptr.limit = sizeof(idt_entry_t)*256-1;
	idt_ptr.base = (Uint32)&idt_ents;
	/* we have to zero everything out */
	memset((Uint8*)&idt_ents[0],0,sizeof(idt_entry_t)*256);
	irq_clearhandlers();
	/* remap PICs so IRQs use IDT gates 32-47 */
	/* this code will look confusing, sorry for the inconvenience */
	outport_b(0x20,0x11);
	outport_b(0xA0,0x11);
	outport_b(0x21,0x20);
	outport_b(0xA1,0x28);
	outport_b(0x21,0x04);
	outport_b(0xA1,0x02);
	outport_b(0x21,0x01);
	outport_b(0xA1,0x01);
	outport_b(0x21,0x00);
	outport_b(0xA1,0x00);
	/* setting up gates */
	/* ISRs */
	idt_setgate(0,(Uint32)isr0,0x08,0x8E);
	idt_setgate(1,(Uint32)isr1,0x08,0x8E);
	idt_setgate(2,(Uint32)isr2,0x08,0x8E);
	idt_setgate(3,(Uint32)isr3,0x08,0x8E);
	idt_setgate(4,(Uint32)isr4,0x08,0x8E);
	idt_setgate(5,(Uint32)isr5,0x08,0x8E);
	idt_setgate(6,(Uint32)isr6,0x08,0x8E);
	idt_setgate(7,(Uint32)isr7,0x08,0x8E);
	idt_setgate(8,(Uint32)isr8,0x08,0x8E);
	idt_setgate(9,(Uint32)isr9,0x08,0x8E);
	idt_setgate(10,(Uint32)isr10,0x08,0x8E);
	idt_setgate(11,(Uint32)isr11,0x08,0x8E);
	idt_setgate(12,(Uint32)isr12,0x08,0x8E);
	idt_setgate(13,(Uint32)isr13,0x08,0x8E);
	idt_setgate(14,(Uint32)isr14,0x08,0x8E);
	idt_setgate(15,(Uint32)isr15,0x08,0x8E);
	idt_setgate(16,(Uint32)isr16,0x08,0x8E);
	idt_setgate(17,(Uint32)isr17,0x08,0x8E);
	idt_setgate(18,(Uint32)isr18,0x08,0x8E);
	idt_setgate(19,(Uint32)isr19,0x08,0x8E);
	idt_setgate(20,(Uint32)isr20,0x08,0x8E);
	idt_setgate(21,(Uint32)isr21,0x08,0x8E);
	idt_setgate(22,(Uint32)isr22,0x08,0x8E);
	idt_setgate(23,(Uint32)isr23,0x08,0x8E);
	idt_setgate(24,(Uint32)isr24,0x08,0x8E);
	idt_setgate(25,(Uint32)isr25,0x08,0x8E);
	idt_setgate(26,(Uint32)isr26,0x08,0x8E);
	idt_setgate(27,(Uint32)isr27,0x08,0x8E);
	idt_setgate(28,(Uint32)isr28,0x08,0x8E);
	idt_setgate(29,(Uint32)isr29,0x08,0x8E);
	idt_setgate(30,(Uint32)isr30,0x08,0x8E);
	idt_setgate(31,(Uint32)isr31,0x08,0x8E);
	/* IRQs */
	idt_setgate(32,(Uint32)irq0,0x08,0x8E);
	idt_setgate(33,(Uint32)irq1,0x08,0x8E);
	idt_setgate(34,(Uint32)irq2,0x08,0x8E);
	idt_setgate(35,(Uint32)irq3,0x08,0x8E);
	idt_setgate(36,(Uint32)irq4,0x08,0x8E);
	idt_setgate(37,(Uint32)irq5,0x08,0x8E);
	idt_setgate(38,(Uint32)irq6,0x08,0x8E);
	idt_setgate(39,(Uint32)irq7,0x08,0x8E);
	idt_setgate(40,(Uint32)irq8,0x08,0x8E);
	idt_setgate(41,(Uint32)irq9,0x08,0x8E);
	idt_setgate(42,(Uint32)irq10,0x08,0x8E);
	idt_setgate(43,(Uint32)irq11,0x08,0x8E);
	idt_setgate(44,(Uint32)irq12,0x08,0x8E);
	idt_setgate(45,(Uint32)irq13,0x08,0x8E);
	idt_setgate(46,(Uint32)irq14,0x08,0x8E);
	idt_setgate(47,(Uint32)irq15,0x08,0x8E);
	/* flush~ */
	idt_flush((Uint32)&idt_ptr);
}
/* set the values of a IDT entry */
static void idt_setgate( Uint8 num, Uint32 bas, Uint16 sel, Uint8 flg )
{
	idt_ents[num].base_l = bas&0xFFFF;
	idt_ents[num].base_h = (bas>>16)&0xFFFF;
	idt_ents[num].sel = sel;
	idt_ents[num].zero = 0;
	idt_ents[num].flags = flg/*|0x60*/; /* <- not yet */
}
