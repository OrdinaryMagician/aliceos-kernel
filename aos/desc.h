#ifndef DESC_H
#define DESC_H
#include "types.h"
struct gdt_entry_struct
{
	Uint16 limit_low;
	Uint16 base_low;
	Uint8 base_middle;
	Uint8 access;
	Uint8 granularity;
	Uint8 base_high;
} __attribute__ ((packed));
typedef struct gdt_entry_struct gdt_entry_t;

struct gdt_ptr_struct
{
	Uint16 limit;
	Uint32 base;
} __attribute__ ((packed));
typedef struct gdt_ptr_struct gdt_ptr_t;

void init_descriptor_tables( void );

struct idt_entry_struct
{
	Uint16 base_lo;
	Uint16 sel;
	Uint8 always0;
	Uint8 flags;
	Uint16 base_hi;
} __attribute__ ((packed));
typedef struct idt_entry_struct idt_entry_t;

struct idt_ptr_struct
{
	Uint16 limit;
	Uint32 base;
} __attribute__ ((packed));
typedef struct idt_ptr_struct idt_ptr_t;

extern void isr0( void );
extern void isr1( void );
extern void isr2( void );
extern void isr3( void );
extern void isr4( void );
extern void isr5( void );
extern void isr6( void );
extern void isr7( void );
extern void isr8( void );
extern void isr9( void );
extern void isr10( void );
extern void isr11( void );
extern void isr12( void );
extern void isr13( void );
extern void isr14( void );
extern void isr15( void );
extern void isr16( void );
extern void isr17( void );
extern void isr18( void );
extern void isr19( void );
extern void isr20( void );
extern void isr21( void );
extern void isr22( void );
extern void isr23( void );
extern void isr24( void );
extern void isr25( void );
extern void isr26( void );
extern void isr27( void );
extern void isr28( void );
extern void isr29( void );
extern void isr30( void );
extern void isr31( void );
extern void irq0( void );
extern void irq1( void );
extern void irq2( void );
extern void irq3( void );
extern void irq4( void );
extern void irq5( void );
extern void irq6( void );
extern void irq7( void );
extern void irq8( void );
extern void irq9( void );
extern void irq10( void );
extern void irq11( void );
extern void irq12( void );
extern void irq13( void );
extern void irq14( void );
extern void irq15( void );
#endif
