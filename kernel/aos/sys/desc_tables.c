/*
	desc_tables.c : definitions for descriptor tables.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/types.h>
#include <sys/desc_tables.h>
#include <printk.h>
#include <sys/helpers.h>
#include <memops.h>
#include <sys/regs.h>
#include <sys/irq_handlers.h>
#include <sys/isr_handlers.h>
#include <sys/port.h>
#include <sys/isr.h>
#include <sys/irq.h>
/* Write the actual GDT pointer */
extern void gdt_flush( uint32_t ptr );
/* Write the actual IDT pointer */
extern void idt_flush( uint32_t ptr );
/* start up GDT */
static void init_gdt( void );
/* set the values of a GDT entry */
static void gdt_setgate( int32_t num, uint32_t bas, uint32_t lim, uint8_t acc,
			 uint8_t grn );
/* start up IDT */
static void init_idt( void );
/* set the values of a IDT entry */
static void idt_setgate( uint8_t num, uint32_t bas, uint16_t sel, uint8_t flg );
/* table arrays */
static gdt_entry_t gdt_ents[5];
static gdt_ptr_t gdt_ptr;
static idt_entry_t idt_ents[256];
static idt_ptr_t idt_ptr;
/* Initialize descriptor tables (duh) */
void init_descriptor_tables( void )
{
	/* global descriptor table */
	printk("Initializing GDT\n");
	init_gdt();
	/* interrupt descriptor table */
	printk("Initializing IDT\n");
	init_idt();
	/* now we can start interrupts */
	printk("Enabling interrupts\n");
	int_enable();
}
/* start up GDT */
static void init_gdt( void )
{
	gdt_ptr.limit = (sizeof(gdt_entry_t)*5)-1;
	gdt_ptr.base = (uint32_t)&gdt_ents;
	/* segments (null, code, data, user mode code, user mode data) */
	printk(" Segment descriptors\n");
	gdt_setgate(0,0,0x00000000,0x00,0x00);
	gdt_setgate(1,0,0xFFFFFFFF,0x9A,0xCF);
	gdt_setgate(2,0,0xFFFFFFFF,0x92,0xCF);
	gdt_setgate(3,0,0xFFFFFFFF,0xFA,0xCF);
	gdt_setgate(4,0,0xFFFFFFFF,0xF2,0xCF);
	/* flush~ */
	printk(" Flushing GDT\n");
	gdt_flush((uint32_t)&gdt_ptr);
}
/* set the values of a GDT entry */
static void gdt_setgate( int32_t num, uint32_t bas, uint32_t lim, uint8_t acc,
			 uint8_t grn )
{
	printk("  %1d [%#08x-%#08x,%#02x,%#02x]\n",num,bas,lim,acc,grn);
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
	idt_ptr.base = (uint32_t)&idt_ents;
	/* we have to zero everything out */
	memset(&idt_ents[0],0,sizeof(idt_entry_t)*256);
	irq_clearhandlers();
	isr_clearhandlers();
	/* remap PICs so IRQs use IDT gates 32-47 */
	/* this code will look confusing, sorry for the inconvenience */
	printk(" Remapping PIC (IRQs on IDT gates 32-47)\n");
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
	printk(" Interrupt Service Routines\n");
	idt_setgate(0,(uint32_t)isr0,0x08,0x8E);
	idt_setgate(1,(uint32_t)isr1,0x08,0x8E);
	idt_setgate(2,(uint32_t)isr2,0x08,0x8E);
	idt_setgate(3,(uint32_t)isr3,0x08,0x8E);
	idt_setgate(4,(uint32_t)isr4,0x08,0x8E);
	idt_setgate(5,(uint32_t)isr5,0x08,0x8E);
	idt_setgate(6,(uint32_t)isr6,0x08,0x8E);
	idt_setgate(7,(uint32_t)isr7,0x08,0x8E);
	idt_setgate(8,(uint32_t)isr8,0x08,0x8E);
	idt_setgate(9,(uint32_t)isr9,0x08,0x8E);
	idt_setgate(10,(uint32_t)isr10,0x08,0x8E);
	idt_setgate(11,(uint32_t)isr11,0x08,0x8E);
	idt_setgate(12,(uint32_t)isr12,0x08,0x8E);
	idt_setgate(13,(uint32_t)isr13,0x08,0x8E);
	idt_setgate(14,(uint32_t)isr14,0x08,0x8E);
	idt_setgate(15,(uint32_t)isr15,0x08,0x8E);
	idt_setgate(16,(uint32_t)isr16,0x08,0x8E);
	idt_setgate(17,(uint32_t)isr17,0x08,0x8E);
	idt_setgate(18,(uint32_t)isr18,0x08,0x8E);
	idt_setgate(19,(uint32_t)isr19,0x08,0x8E);
	idt_setgate(20,(uint32_t)isr20,0x08,0x8E);
	idt_setgate(21,(uint32_t)isr21,0x08,0x8E);
	idt_setgate(22,(uint32_t)isr22,0x08,0x8E);
	idt_setgate(23,(uint32_t)isr23,0x08,0x8E);
	idt_setgate(24,(uint32_t)isr24,0x08,0x8E);
	idt_setgate(25,(uint32_t)isr25,0x08,0x8E);
	idt_setgate(26,(uint32_t)isr26,0x08,0x8E);
	idt_setgate(27,(uint32_t)isr27,0x08,0x8E);
	idt_setgate(28,(uint32_t)isr28,0x08,0x8E);
	idt_setgate(29,(uint32_t)isr29,0x08,0x8E);
	idt_setgate(30,(uint32_t)isr30,0x08,0x8E);
	idt_setgate(31,(uint32_t)isr31,0x08,0x8E);
	/* IRQs */
	printk(" Interrupt Requests\n");
	idt_setgate(32,(uint32_t)irq0,0x08,0x8E);
	idt_setgate(33,(uint32_t)irq1,0x08,0x8E);
	idt_setgate(34,(uint32_t)irq2,0x08,0x8E);
	idt_setgate(35,(uint32_t)irq3,0x08,0x8E);
	idt_setgate(36,(uint32_t)irq4,0x08,0x8E);
	idt_setgate(37,(uint32_t)irq5,0x08,0x8E);
	idt_setgate(38,(uint32_t)irq6,0x08,0x8E);
	idt_setgate(39,(uint32_t)irq7,0x08,0x8E);
	idt_setgate(40,(uint32_t)irq8,0x08,0x8E);
	idt_setgate(41,(uint32_t)irq9,0x08,0x8E);
	idt_setgate(42,(uint32_t)irq10,0x08,0x8E);
	idt_setgate(43,(uint32_t)irq11,0x08,0x8E);
	idt_setgate(44,(uint32_t)irq12,0x08,0x8E);
	idt_setgate(45,(uint32_t)irq13,0x08,0x8E);
	idt_setgate(46,(uint32_t)irq14,0x08,0x8E);
	idt_setgate(47,(uint32_t)irq15,0x08,0x8E);
	/* Syscalls */
	printk(" Syscalls\n");
	idt_setgate(128,(uint32_t)isr128,0x08,0x8E);
	/* flush~ */
	printk(" Flushing IDT\n");
	idt_flush((uint32_t)&idt_ptr);
}
/* set the values of a IDT entry */
static void idt_setgate( uint8_t num, uint32_t bas, uint16_t sel, uint8_t flg )
{
	printk("  %3d [%#08x,%#04x,%#02x]\n",num,bas,sel,flg);
	idt_ents[num].base_l = bas&0xFFFF;
	idt_ents[num].base_h = (bas>>16)&0xFFFF;
	idt_ents[num].sel = sel;
	idt_ents[num].zero = 0;
	idt_ents[num].flags = flg/*|0x60*/; /* <- not yet */
}
