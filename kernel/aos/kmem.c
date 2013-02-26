/*
	kmem.c : Memory allocation.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <kmem.h>
#include <sys/helpers.h>
#include <printk.h>
#include <berp.h>

/* placement address */
static Uint32 p_addr_init;
static Uint32 p_addr;
/* max address */
static Uint32 m_addr;

/* allocation stats */
static Uint32 alloc_count;
static Uint32 alloc_mem;

/* global allocation function */
static Uint32 kmalloc_global( Uint32 sz, Uint8 algn, Uint32 *phys )
{
	if ( p_addr+sz >= m_addr )
	{
		printk("Could not allocate %u bytes\n",sz);
		BERP("Out of memory");
	}
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
	alloc_count++;
	alloc_mem += p_addr-p_addr_old;
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

/* free allocated memory (does nothing if there's no heap) */
void kfree( Uint32 addr )
{
}

/* initialize the memory manager */
void init_kmem( Uint32 iaddr, Uint32 eaddr )
{
	p_addr_init = iaddr;
	p_addr = p_addr_init;
	m_addr = eaddr;
	alloc_count = 0;
	alloc_mem = 0;
}

/* current addr variables values */
void kmem_addrs( Uint32 *pai, Uint32 *pa, Uint32 *ma )
{
	if ( pai )
		*pai = p_addr_init;
	if ( pa )
		*pa = p_addr;
	if ( ma )
		*ma = m_addr;
}
