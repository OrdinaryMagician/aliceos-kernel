/*
	desc_tables.h : definitions for descriptor tables.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef DESC_TABLES_H
#define DESC_TABLES_H
#include <sys/types.h>
#include <sys/helpers.h>
/* GDT entry struct */
typedef struct
{
	uint16_t limit_l;
	uint16_t base_l;
	uint8_t base_m;
	uint8_t access;
	uint8_t granularity;
	uint8_t base_h;
} gdt_entry_t;
/*
	GDT access byte format:
	0-3 Segment type
	4 Descriptor type
	5-6 Descriptor privilege level (Ring 0-3)
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
	uint16_t limit;
	uint32_t base;
} gdt_ptr_t;
/* IDT entry struct */
typedef struct
{
	uint16_t base_l;
	uint16_t sel;
	uint8_t zero;
	uint8_t flags;
	uint16_t base_h;
} idt_entry_t;
/*
	IDT flags byte format:
	0-4 Always 00110
	5-6 Privilege level
	7 Present?
*/
/* IDT array pointer struct */
typedef struct
{
	uint16_t limit;
	uint32_t base;
} idt_ptr_t;
/* Initialize descriptor tables (duh) */
void init_descriptor_tables( void );
#endif
