/*
	kmem.c : Static memory allocation.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <kmem.h>
#include <sys/helpers.h>
#include <printk.h>
#include <berp.h>
#include <sys/paging.h>
#include <kdmem.h>
/* in sys/paging.c */
extern pdir_t *kernel_directory;
/* in sys/pagingasm.s */
extern uint8_t pagingenabled( void );
/* in kdmem.c */
extern uint8_t kdmem_enabled;
/* placement address */
uint32_t p_addr_init = 0;
uint32_t p_addr = 0;
/* max address */
uint32_t m_addr = 0;
/* allocation stats */
uint32_t alloc_count = 0;
uint32_t alloc_mem = 0;
/* gaps to skip (start, end) */
typedef struct
{
	uint32_t start, end;
} memgap_t;
memgap_t a_skip[32];
uint8_t n_skip = 0;
/* global allocation function */
void *kmalloc_global( uint32_t sz, uint8_t alg, uint32_t *phys )
{
	if ( kdmem_enabled )
		return kdalloc_global(sz,alg,phys);
	uint32_t p_addr_veryold = p_addr;
	/* page-align address in case it's not already */
	if ( alg && p_addr&(0xFFFFF000) )
		p_addr = (p_addr&0xFFFFF000)+0x1000;
	/* skip any gaps */
	uint8_t i;
	for ( i=0; i<n_skip; i++ )
	{
		if ( (p_addr >= a_skip[i].start) && (p_addr <= a_skip[i].end) )
			p_addr = a_skip[i].end+1;
		/* check if we need to realign */
		if ( alg && p_addr&(0xFFFFF000) )
			p_addr = (p_addr&0xFFFFF000)+0x1000;
	}
	if ( p_addr+sz >= m_addr )
	{
		printk("Could not allocate %u bytes\n",sz);
		/* if the placement allocator can't work, it's all over */
		BERP("Placement allocator out of memory");
	}
	if ( phys )
	{
		if ( pagingenabled() )
		{
			page_t *pg = get_page(p_addr,0,kernel_directory);
			*phys = pg->frame*0x1000+(p_addr&0xFFF);
		}
		else
			*phys = p_addr;
	}
	uint32_t p_addr_old = p_addr;
	p_addr += sz;
	alloc_count++;
	alloc_mem += p_addr-p_addr_veryold;
	return (void*)p_addr_old;
}
/* page-aligned kmalloc */
void *kmalloc_a( uint32_t sz )
{
	return kmalloc_global(sz,1,0);
}
/* page-aligned physical kmalloc */
void *kmalloc_ap( uint32_t sz, uint32_t *phys )
{
	return kmalloc_global(sz,1,phys);
}
/* physical kmalloc */
void *kmalloc_p( uint32_t sz, uint32_t *phys )
{
	return kmalloc_global(sz,0,phys);
}
/* the lite version */
void *kmalloc( uint32_t sz )
{
	return kmalloc_global(sz,0,0);
}
/* global reallocation function */
void *krealloc_global( void *prev, uint32_t sz, uint8_t alg, uint32_t *phys )
{
	if ( kdmem_enabled )
		return kdrealloc_global(prev,sz,alg,phys);
	return kmalloc_global(sz,alg,phys);
}
/* page-aligned krealloc */
void *krealloc_a( void *prev, uint32_t sz )
{
	return krealloc_global(prev,sz,1,0);
}
/* page-aligned physical krealloc */
void *krealloc_ap( void *prev, uint32_t sz, uint32_t *phys )
{
	return krealloc_global(prev,sz,1,phys);
}
/* physical krealloc */
void *krealloc_p( void *prev, uint32_t sz, uint32_t *phys )
{
	return krealloc_global(prev,sz,0,phys);
}
/* the lite version */
void *krealloc( void *prev, uint32_t sz )
{
	return krealloc_global(prev,sz,0,0);
}
/* does nothing unless dynamic allocator is initiated */
void kfree( void *a )
{
	if ( kdmem_enabled )
		kdfree(a);
}
/* initialize the memory manager */
void init_kmem( uint32_t iaddr, uint32_t eaddr )
{
	/* skip any gaps */
	uint8_t i;
	for ( i=0; i<n_skip; i++ )
		if ( (iaddr >= a_skip[i].start) && (iaddr <= a_skip[i].end) )
			iaddr = a_skip[i].end+1;
	p_addr_init = iaddr;
	p_addr = p_addr_init;
	m_addr = eaddr;
}
/* add a memory gap to skip */
void kmem_addgap( uint32_t start, uint32_t end )
{
	if ( n_skip == 31 )
		return;	/* TOO MANY ALREADY */
	/* TODO handle overlapping areas */
	a_skip[n_skip].start = start;
	a_skip[n_skip].end = end;
	n_skip++;
}
