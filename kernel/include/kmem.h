/*
	kmem.h : Static memory allocation.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef KMEM_H
#define KMEM_H
#include <sys/types.h>
/* memory allocation functions */
Uint32 kmalloc_global( Uint32 sz, Uint8 alg, Uint32 *phys );
Uint32 kmalloc_a( Uint32 sz );
Uint32 kmalloc_ap( Uint32 sz, Uint32 *phys );
Uint32 kmalloc_p( Uint32 sz, Uint32 *phys );
Uint32 kmalloc( Uint32 sz );
Uint32 krealloc_global( Uint32 prev, Uint32 sz, Uint8 alg, Uint32 *phys );
Uint32 krealloc_a( Uint32 prev, Uint32 sz );
Uint32 krealloc_ap( Uint32 prev, Uint32 sz, Uint32 *phys );
Uint32 krealloc_p( Uint32 prev, Uint32 sz, Uint32 *phys );
Uint32 krealloc( Uint32 prev, Uint32 sz );
void kfree( Uint32 a );

/* initialize the memory manager */
void init_kmem( Uint32 iaddr, Uint32 eaddr );
/* add a memory gap to skip */
void kmem_addgap( Uint32 start, Uint32 end );
/* current addr variables values */
void kmem_addrs( Uint32 *pai, Uint32 *pa, Uint32 *ma );
#endif
