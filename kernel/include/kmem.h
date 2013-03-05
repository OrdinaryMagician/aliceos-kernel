/*
	kmem.h : Static memory allocation.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef KMEM_H
#define KMEM_H
#include <sys/types.h>
/* page-aligned kmalloc */
Uint32 kmalloc_a( Uint32 sz );
/* page-aligned physical kmalloc */
Uint32 kmalloc_ap( Uint32 sz, Uint32 *phys );
/* physical kmalloc */
Uint32 kmalloc_p( Uint32 sz, Uint32 *phys );
/* the lite version */
Uint32 kmalloc( Uint32 sz );
/* initialize the memory manager */
void init_kmem( Uint32 iaddr, Uint32 eaddr );
/* add a memory gap to skip */
void kmem_addgap( Uint32 start, Uint32 end );
/* current addr variables values */
void kmem_addrs( Uint32 *pai, Uint32 *pa, Uint32 *ma );
#endif
