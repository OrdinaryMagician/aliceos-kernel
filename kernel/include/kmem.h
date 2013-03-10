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
void *kmalloc_global( uint32_t sz, uint8_t alg, uint32_t *phys );
void *kmalloc_a( uint32_t sz );
void *kmalloc_ap( uint32_t sz, uint32_t *phys );
void *kmalloc_p( uint32_t sz, uint32_t *phys );
void *kmalloc( uint32_t sz );
void *krealloc_global( void *prev, uint32_t sz, uint8_t alg, uint32_t *phys );
void *krealloc_a( void *prev, uint32_t sz );
void *krealloc_ap( void *prev, uint32_t sz, uint32_t *phys );
void *krealloc_p( void *prev, uint32_t sz, uint32_t *phys );
void *krealloc( void *prev, uint32_t sz );
void kfree( void *a );
/* initialize the memory manager */
void init_kmem( uint32_t iaddr, uint32_t eaddr );
/* add a memory gap to skip */
void kmem_addgap( uint32_t start, uint32_t end );
/* current addr variables values */
void kmem_addrs( uint32_t *pai, uint32_t *pa, uint32_t *ma );
#endif
