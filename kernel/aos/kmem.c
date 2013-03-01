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

/* in sys/paging.c */
extern page_directory_t *kernel_directory;
/* in sys/pagingasm.s */
extern Uint32 pagingenabled( void );

/* placement address */
static Uint32 p_addr_init = 0;
static Uint32 p_addr = 0;
/* max address */
static Uint32 m_addr = 0;

/* allocation stats */
static Uint32 alloc_count = 0;
static Uint32 alloc_mem = 0;

/* gaps to skip (start, end) */
typedef struct
{
	Uint32 start, end;
} memgap_t;
memgap_t a_skip[32];
Uint8 n_skip = 0;

/* global allocation function */
static Uint32 kmalloc_global( Uint32 sz, Uint8 algn, Uint32 *phys )
{
	Uint32 p_addr_veryold = p_addr;
	/* page-align address in case it's not already */
	if ( algn && p_addr&(0xFFFFF000) )
	{
		p_addr &= 0xFFFFF000;
		p_addr += 0x1000;
	}
	/* skip any gaps */
	Uint8 i;
	for ( i=0; i<n_skip; i++ )
	{
		if ( (p_addr >= a_skip[i].start) && (p_addr <= a_skip[i].end) )
			p_addr = a_skip[i].end+1;
		/* check if we need to realign */
		if ( algn && p_addr&(0xFFFFF000) )
		{
			p_addr &= 0xFFFFF000;
			p_addr += 0x1000;
		}
	}
	if ( p_addr+sz >= m_addr )
	{
		printk("Could not allocate %u bytes\n",sz);
		BERP("Out of memory");
	}
	if ( phys )
	{
		if ( pagingenabled() )
		{
			page_t *pg = get_page(p_addr,0,kernel_directory);
			*phys = pg->frame*0x1000+p_addr&0xFFF;
		}
		else
			*phys = p_addr;
	}
	Uint32 p_addr_old = p_addr;
	p_addr += sz;
	alloc_count++;
	alloc_mem += p_addr-p_addr_veryold;
	return p_addr_old;
}

/* page-aligned kmalloc */
Uint32 kmalloc_a( Uint32 sz )
{
	return kmalloc_global(sz,1,0);
}

/* page-aligned physical kmalloc */
Uint32 kmalloc_ap( Uint32 sz, Uint32 *phys )
{
	return kmalloc_global(sz,1,phys);
}

/* physical kmalloc */
Uint32 kmalloc_p( Uint32 sz, Uint32 *phys )
{
	return kmalloc_global(sz,0,phys);
}

/* the lite version */
Uint32 kmalloc( Uint32 sz )
{
	return kmalloc_global(sz,0,0);
}

/* initialize the memory manager */
void init_kmem( Uint32 iaddr, Uint32 eaddr )
{
	/* skip any gaps */
	Uint8 i;
	for ( i=0; i<n_skip; i++ )
		if ( (iaddr >= a_skip[i].start) && (iaddr <= a_skip[i].end) )
			iaddr = a_skip[i].end+1;
	p_addr_init = iaddr;
	p_addr = p_addr_init;
	m_addr = eaddr;
}

/* add a memory gap to skip */
void kmem_addgap( Uint32 start, Uint32 end )
{
	if ( n_skip == 31 )
		return;	/* TOO MANY ALREADY */
	/* TODO handle overlapping areas */
	a_skip[n_skip].start = start;
	a_skip[n_skip].end = end;
	n_skip++;
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

/* return number of allocations performed */
Uint32 kmem_nalloc( void )
{
	return alloc_count;
}

/* return total, used and free memory */
Uint32 kmem_total( void )
{
	Uint32 mm = m_addr;
	Uint8 i;
	for ( i=0; i<n_skip; i++ )
		mm -= a_skip[i].end-a_skip[i].start;
	return mm;
}

Uint32 kmem_used( void )
{
	return alloc_mem;
}

Uint32 kmem_free( void )
{
	return kmem_total()-alloc_mem;
}
