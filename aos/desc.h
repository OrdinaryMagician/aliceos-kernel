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
}
