/*
	kdmem.h : Dynamic memory allocation.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef KDMEM_H
#define KDMEM_H
#include <sys/types.h>
/* reserved list size */
/* NOTE: 131072 allocations are possible per MiB */
#define KDMEM_RESV 0x100000 /* 1MiB */
#define KDMEM_ST 0xC0000000 /* virtual start address */
#define KDMEM_SIZ 0x4000000 /* 64MiB */
/* memory block structure */
typedef struct
{
	uint32_t start;
	uint32_t end;
} memblk_t;
/* reserve a memory area */
/* generic */
void *kdalloc_global( uint32_t sz, uint8_t alg, uint32_t *phys );
/* vanilla */
void *kdalloc( uint32_t sz );
/* page-aligned */
void *kdalloc_a( uint32_t sz );
/* return physical address */
void *kdalloc_p( uint32_t sz, uint32_t *phys );
/* page-aligned and return physical address */
void *kdalloc_ap( uint32_t sz, uint32_t *phys );
/* reallocate (resize) a memory area */
/* generic */
void *kdrealloc_global( void *prev, uint32_t sz, uint8_t alg, uint32_t *phys );
/* vanilla */
void *kdrealloc( void *prev, uint32_t sz );
/* page-aligned */
void *kdrealloc_a( void *prev, uint32_t sz );
/* return physical address */
void *kdrealloc_p( void *prev, uint32_t sz, uint32_t *phys );
/* page-aligned and return physical address */
void *kdrealloc_ap( void *prev, uint32_t sz, uint32_t *phys );
/* free a memory area */
void kdfree( void *a );
/* retrieve used blocks */
uint32_t kdmem_count( void );
/* retrieve total memory used */
uint32_t kdmem_amount( void );
/* initialize dynamic memory allocator */
void kdmem_init( uint32_t start, uint32_t size, uint32_t psize );
#endif
