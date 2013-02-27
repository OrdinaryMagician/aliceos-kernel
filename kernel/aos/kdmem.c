/*
	kdmem.c : Dynamic memory allocation.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <kdmem.h>
#include <kmem.h>
#include <berp.h>
#include <memops.h>
#include <paging.h>

/* in sys/paging.c */
extern page_directory_t *kernel_directory = NULL;
/* in sys/pagingasm.s */
extern Uint8 pagingenabled( void );

/*
	How this memory model works:

	The used memory is managed in a pile of blocks.

	Every time there's an allocation, the manager checks
	if there's any unused gaps inside kdmem_range where the
	requested amount of space can fit. The new block is
	stacked onto the pile and becomes used memory. If there's
	no space available the allocation function returns 0.

	Every time a memory block is freed, said block is deleted
	and any blocks above it are pushed down.
	
	After some time, the pile will end up with blocks that aren't
	in order. When this happens, the allocator might work slowly.
	It's recommended to schedule a task to call kdmem_tidy() every
	once in a while, to rearrange the pile.
	
	This model is ridiculously inefficient, but hell, at least it's
	easy to implement.
*/

/* memory block pile */
static memblk_t *kdmem_pile = NULL;
/* total (virtual) memory range available for allocation */
static memblk_t kdmem_range = {0x00000000,0xFFFFFFFF};
/* pile sizes */
static Uint32 kdmem_psize = 0;
static Uint32 kdmem_psize_max = 0;

/* find the first gap where we can fit a specific block */
static Uint32 mblk_findgap( Uint32 sz, Uint8 algn )
{
}

/* add a memory block to the pile */
static Uint8 mblk_add( Uint32 start, Uint32 end )
{
}

/* remove a memory block from the pile */
static Uint8 mblk_rm( Uint32 idx )
{
	/* if there are no blocks reserved yet */
	if ( !kdmem_psize )
		return 0;
	/* if we're trying to go beyond index */
	if ( idx >= kdmem_psize )
		return 0;
	memset(kdmem_pile+idx,0,sizeof(memblk_t)); /* zero out this block entry */
	kdmem_psize--;
	if ( idx < kdmem_psize ) /* push back anything on top if needed */
		memmove(kdmem_pile+idx+1,kdmem_pile+idx,sizeof(memblk_t)*(kdmem_psize-idx));
	return 1;
}

/* find the memory block that starts at a specific address */
static Uint32 mblk_find( Uint32 addr )
{
	Uint32 i;
	for ( i=0; i<kdmem_psize; i++ )
		if ( kdmem_pile[i].start == addr )
			return i;
	return Uint32_MAX;
}

/* reserve a memory area */
Uint32 kdalloc_int( Uint32 sz, Uint8 algn, Uint32 *phys ) /* internal */
{
	Uint32 addr = mblk_add(sz,algn);
	if ( !addr )
		return 0; /* could not allocate */
	if ( phys )
	{
		if ( pagingenabled() )
		{
			page_t *pg = get_page(addr,0,kernel_directory);
			*phys = pg->frame*0x1000+addr&0xFFF;
		}
		else
			*phys = addr;
	}
	return addr;
}

Uint32 kdalloc( Uint32 sz ) /* vanilla */
{
	return kdalloc_int(sz,0,NULL);
}

Uint32 kdalloc_a( Uint32 sz ) /* page-aligned */
{
	return kdalloc_int(sz,1,NULL);
}

Uint32 kdalloc_p( Uint32 sz, Uint32 *phys ) /* return physical address */
{
	return kdalloc_int(sz,0,phys);
}

Uint32 kdalloc_ap( Uint32 sz, Uint32 *phys ) /* page-aligned and return physical address */
{
	return kdalloc_int(sz,1,phys);
}

/* free a memory area */
void kdfree( Uint32 a )
{
	Uint32 blk = mblk_find(a);
	if ( blk == UINT32_MAX )
		return; /* just ignore everything */
	mblk_rm(blk);
}

/* retrieve used blocks */
Uint32 kdmem_count( void )
{
	return kdmem_psize;
}

/* retrieve total memory used */
Uint32 kdmem_amount( void )
{
	Uint32 i;
	Uint32 mem = 0;
	for ( i=0; i<kdmem_psize; i++ )
		mem += kdmem_pile[i].end-kdmem_pile[i].start;
	return mem;
}

/* issue a reorder of the memory block pile */
void kdmem_tidy( void )
{
	/* TODO */
}

/* initialize dynamic memory allocator */
void kdmem_init( Uint32 start, Uint32 end, Uint32 psize )
{
	/* sanity checks */
	if ( kdmem_range.end <= kdmem_range.start )
		BERP("Attempted to initialize allocator with zero or negative reserved area");
	if ( !psize < sizeof(memblk_t) )
		BERP("Pile size is too small");
	kdmem_range.start = start;
	kdmem_range.end = end;
	/* use the static (placement) allocator to reserve our pile */
	Uint32 kdmem_psize_max = psize/sizeof(memblk_t);
	kdmem_pile = (memblk_t*)kalloc_a(kdmem_psize_max*sizeof(memblk_t));
	memset((Uint8*)kdmem_pile,0,kdmem_psize_max*sizeof(memblk_t));
	kdmem_psize = 0;
}
