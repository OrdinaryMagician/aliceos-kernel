/*
	kmem.h : Memory allocation.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef KMEM_H
#define KMEM_H
#include <sys/types.h>
/* page-aligned kmalloc */
Uint32 kmalloc_a( Uint32 sz );
/* kmalloc returning a physical address */
Uint32 kmalloc_p( Uint32 sz, Uint32 *phys );
/* page-aligned kmalloc returning a physical address */
Uint32 kmalloc_ap( Uint32 sz, Uint32 *phys );
/* the lite version */
Uint32 kmalloc( Uint32 sz );
/* initialize the memory manager */
void init_kmem( void );
/* add a memory gap to skip */
void kmem_addgap( Uint32 start, Uint32 end );
/* memory statistics: start, end and number of allocations */
void memstat( Uint32 *st, Uint32 *en, Uint32 *ct );
#endif
