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
/* free allocated memory (does nothing if there's no heap) */
void kfree( Uint32 addr );
/* initialize the memory manager */
void init_kmem( Uint32 iaddr, Uint32 eaddr );
/* current addr variables values */
void kmem_addrs( Uint32 *pai, Uint32 *pa, Uint32 *ma );
/* memory usage: free, used and total */
void kmem_use( Uint32 *fmem, Uint32 *umem, Uint32 *tmem );
#endif
