/*
	kmem.c : Memory allocation.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <kmem.h>

/* placement address (initial and current) */
static Uint32 p_addr_init;
static Uint32 p_addr;
static Uint32 alloc_count;

/* global allocation function */
static Uint32 kmalloc_global( Uint32 sz, Uint8 algn, Uint32 *phys )
{
	alloc_count++;
	/* page-align address in case it's not already */
	if ( algn && p_addr&(0xFFFFF000) )
	{
		p_addr &= 0xFFFFF000;
		p_addr += 0x1000;
	}
	if ( phys )
		*phys = p_addr;
	Uint32 p_addr_old = p_addr;
	p_addr += sz;
	return p_addr_old;
}

/* page-aligned kmalloc */
Uint32 kmalloc_a( Uint32 sz )
{
	return kmalloc_global(sz,1,0);
}

/* kmalloc returning a physical address */
Uint32 kmalloc_p( Uint32 sz, Uint32 *phys )
{
	return kmalloc_global(sz,0,phys);
}

/* page-aligned kmalloc returning a physical address */
Uint32 kmalloc_ap( Uint32 sz, Uint32 *phys )
{
	return kmalloc_global(sz,1,phys);
}

/* the lite version */
Uint32 kmalloc( Uint32 sz )
{
	return kmalloc_global(sz,0,0);
}

/* initialize the memory manager */
void init_kmem( Uint32 p )
{
	p_addr = p_addr_init = p;
	alloc_count = 0;
}

/* memory statistics: start, end and number of allocations */
void memstat( Uint32 *st, Uint32 *en, Uint32 *ct )
{
	*st = p_addr_init;
	*en = p_addr;
	*ct = alloc_count;
}
