#ifndef KHEAP_H
#define KHEAP_H
#include "types.h"
#include "ordered_array.h"
#define KHEAP_START        0xC0000000
#define KHEAP_INITIAL_SIZE 0x00100000
#define HEAP_INDEX_SIZE    0x00020000
#define HEAP_MAGIC         0xABADCAFE
#define HEAP_MIN_SIZE      0x00070000
typedef struct
{
	Uint32 magic;
	Uint8 is_hole;
	Uint32 size;
} header_t;
typedef struct
{
	Uint32 magic;
	header_t *header;
} footer_t;
typedef struct
{
	ordered_array_t index;
	Uint32 start_address;
	Uint32 end_address;
	Uint32 max_address;
	Uint8 supervisor;
	Uint8 readonly;
} heap_t;
heap_t *create_heap( Uint32 start, Uint32 end, Uint32 max, Uint8 supervisor, Uint8 readonly );
void *alloc( Uint32 size, Uint8 page_align, heap_t *heap );
void free( void *p, heap_t *heap );
Uint32 kmalloc_int( Uint32 szm, Uint8 align, Uint32 *phys );
Uint32 kmalloc_a( Uint32 sz );
Uint32 kmalloc_p( Uint32 sz, Uint32 *phys );
Uint32 kmalloc_ap( Uint32 sz, Uint32 *phys );
Uint32 kmalloc( Uint32 sz );
void kfree( void *p );
#endif
