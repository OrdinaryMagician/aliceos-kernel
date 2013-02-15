/*
	tables.h : definitions for descriptor tables.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef TABLES_H
#define TABLES_H
#include <sys/types.h>
#include <sys/helpers.h>

/* GDT entry struct */
typedef struct
{
	Uint16 limit_l;
	Uint16 base_l;
	Uint8 base_m;
	Uint8 access;
	Uint8 granularity;
	Uint8 base_h;
} attribute((packed)) gdt_entry_t;

/*

GDT access byte format:

0-3 Segment type
4 Descriptor type
5-6 Descriptor privilege level (Ring 0-3) (check your descriptor pri-*is shot*)
7 Present?

GDT granularity byte format:

0-3 Segment length
4 Always zero (available for system use)
5 Always zero
6 Operand size (0 = 16bit, 1 = 32bit)
7 Granularity (0 = 1B, 1 = 1KB)

*/

/* GDT array pointer struct */
typedef struct
{
	Uint16 limit;
	Uint32 base;
} attribute((packed)) gdt_ptr_t;

/* IDT entry struct */
typedef struct
{
	Uint16 base_l;
	Uint16 sel;
	Uint8 zero;
	Uint8 flags;
	Uint16 base_h;
} attribute((packed)) idt_entry_t;

/*

IDT flags byte format:

0-4 Always 00110
5-6 Privilege level (no, I'm not making that joke again)
7 Present?

*/

/* IDT array pointer struct */
typedef struct
{
	Uint16 limit;
	Uint32 base;
} attribute((packed)) idt_ptr_t;

/* ISR handlers */
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

/* Initialize descriptor tables (duh) */
void init_descriptor_tables( void );
#endif
