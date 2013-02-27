/*
	kdmem.h : Dynamic memory allocation.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef KDMEM_H
#define KDMEM_H
#include <sys/types.h>

/* memory block structure */
typedef struct
{
	Uint32 start; /* start address */
	Uint32 end;   /* end address */
} memblk_t;

/* reserve a memory area */
Uint32 kdalloc( Uint32 sz ); /* vanilla */
Uint32 kdalloc_a( Uint32 sz ); /* page-aligned */
Uint32 kdalloc_p( Uint32 sz, Uint32 *phys ); /* return physical address */
Uint32 kdalloc_ap( Uint32 sz, Uint32 *phys ); /* page-aligned and return physical address */
/* free a memory area */
void kdfree( Uint32 a );
/* retrieve used blocks */
Uint32 kdmem_count( void );
/* retrieve total memory used */
Uint32 kdmem_amount( void );
/* issue a reorder of the memory block pile */
void kdmem_tidy( void );
/* initialize dynamic memory allocator */
void kdmem_init( Uint32 start, Uint32 end, Uint32 psize );

#endif
