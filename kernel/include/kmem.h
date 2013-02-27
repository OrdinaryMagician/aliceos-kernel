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
/* the lite version */
Uint32 kmalloc( Uint32 sz );
/* initialize the memory manager */
void init_kmem( Uint32 iaddr, Uint32 eaddr );
/* add a memory gap to skip */
void kmem_addgap( Uint32 start, Uint32 end );
/* current addr variables values */
void kmem_addrs( Uint32 *pai, Uint32 *pa, Uint32 *ma );
/* return number of allocations performed */
Uint32 kmem_nalloc( void );
/* return total, used and free memory */
Uint32 kmem_total( void );
Uint32 kmem_used( void );
Uint32 kmem_free( void );
#endif
