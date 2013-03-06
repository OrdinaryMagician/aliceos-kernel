/*
	kdmem.c : Dynamic memory allocation.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <kdmem.h>
#include <sys/paging.h>
#include <kmem.h>
#include <berp.h>
#include <memops.h>

/* in sys/paging.c */
extern page_directory_t *kernel_directory;
/* in sys/pagingasm.s */
extern Uint32 pagingenabled( void );

/*
	How this memory model works:

	The used memory is managed in a list of blocks.

	Every time there's an allocation, the manager checks
	if there's any unused gaps inside kdmem_range where the
	requested amount of space can fit. The new block is
	inserted into the list and becomes used memory. If the
	new block is going to be inserted between already existing
	entries, everything above its new index is pushed forward.
	If there's no space available the allocation function returns 0.

	Every time a memory block is freed, said block is deleted
	and any blocks above it are pushed down.

	This is probably not every efficient. It slows down a lot after
	thousands of allocations (exponentially, even).

	Expanding and shrinking of the memory area not implemented yet.
*/


/* memory block list */
static memblk_t *kdmem_list = NULL;
/* total (virtual) memory range available for allocation */
static Uint32 kdmem_range[2] = {0x00000000,0xFFFFFFFF};
/* pile sizes */
static Uint32 kdmem_psize = 0;
static Uint32 kdmem_psize_max = 0;
/* we're enabled? */
Uint8 kdmem_enabled = 0;

/* find the first gap where we can fit a specific block */
static Uint32 mblk_findgap( Uint32 sz, Uint8 algn, Uint32 *idx )
{
	Uint32 i;
	Uint32 pa = kdmem_range[0], pb = kdmem_range[1];
	if ( algn )
		pa = (pa&0xFFFFF000)+0x1000;
	/* nothing in the list */
	if ( !kdmem_psize )
	{
		if ( idx )
			*idx = 0;
		if ( sz < (pb-pa) )
			return pa;
	}
	/* first check: gaps between memory range and first block */
	pb = kdmem_list[0].start;
	if ( pa < pb )
	{
		if ( idx )
			*idx = 0;
		if ( sz < (pb-pa) )
			return pa;
	}
	/* second check: recursive check gaps between blocks */
	pa = kdmem_list[0].end+1;
	if ( algn )
		pa = (pa&0xFFFFF000)+0x1000;
	for ( i=1; i<kdmem_psize; i++ )
	{
		if ( idx )
			*idx = i;
		pb = kdmem_list[i].start;
		if ( sz < (pb-pa) )
			return pa;
		pa = kdmem_list[i].end+1;
		if ( algn )
			pa = (pa&0xFFFFF000)+0x1000;
	}
	/* third check: free space after last block */
	if ( i == kdmem_psize )
	{
		if ( idx )
			*idx = i;
		pa = (kdmem_list[i-1].end+1);
		if ( algn )
			pa = (pa&0xFFFFF000)+0x1000;
		pb = kdmem_range[1];
		if ( sz < (pb-pa) )
			return pa;
	}
	return 0; /* i-it doesn't fit... */
}

/* add a memory block to the pile */
static Uint8 mblk_add( Uint32 idx, Uint32 start, Uint32 end )
{
	/* list full */
	if ( kdmem_psize >= kdmem_psize_max )
		return 0;
	if ( idx < kdmem_psize ) /* push everything above */
		memmove((Uint8*)(kdmem_list+idx+1),(Uint8*)(kdmem_list+idx),
			sizeof(memblk_t)*(kdmem_psize-idx));
	/* put it on the list */
	kdmem_list[idx].start = start;
	kdmem_list[idx].end = end;
	kdmem_psize++;
	return 1;
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
	/* zero out this block entry */
	memset((Uint8*)(kdmem_list+idx),0,sizeof(memblk_t));
	kdmem_psize--;
	if ( idx < kdmem_psize ) /* push back anything on top if needed */
		memmove((Uint8*)(kdmem_list+idx),(Uint8*)(kdmem_list+idx+1),
			sizeof(memblk_t)*(kdmem_psize-idx));
	return 1;
}

/* find the memory block that starts at a specific address */
static Uint32 mblk_find( Uint32 addr )
{
	Uint32 i;
	for ( i=0; i<kdmem_psize; i++ )
		if ( kdmem_list[i].start == addr )
			return i;
	return UINT32_MAX;
}

/* reserve a memory area */
Uint32 kdalloc_global( Uint32 sz, Uint8 algn, Uint32 *phys ) /* global */
{
	Uint32 idx = 0;
	Uint32 addr = mblk_findgap(sz,algn,&idx);
	if ( !addr )
		return 0; /* no space available */
	if ( !mblk_add(idx,addr,addr+sz) )
		return 0; /* couldn't add block */
	if ( phys )
	{
		if ( pagingenabled() )
		{
			page_t *pg = get_page(addr,0,kernel_directory);
			*phys = pg->frame*0x1000+(addr&0xFFF);
		}
		else
			*phys = addr;
	}
	return addr;
}

/* vanilla */
Uint32 kdalloc( Uint32 sz )
{
	return kdalloc_global(sz,0,NULL);
}

/* page-aligned */
Uint32 kdalloc_a( Uint32 sz )
{
	return kdalloc_global(sz,1,NULL);
}

/* return physical address */
Uint32 kdalloc_p( Uint32 sz, Uint32 *phys )
{
	return kdalloc_global(sz,0,phys);
}

/* page-aligned and return physical address */
Uint32 kdalloc_ap( Uint32 sz, Uint32 *phys )
{
	return kdalloc_global(sz,1,phys);
}

/* reallocate (resize) a memory area */
Uint32 kdrealloc_global( Uint32 prev, Uint32 newsz, Uint8 algn, Uint32 *phys ) /* generic */
{
	Uint32 blk = mblk_find(prev);
	if ( blk == UINT32_MAX )
		return 0; /* no block at this address */
	Uint32 osz = kdmem_list[blk].end-kdmem_list[blk].start;
	if ( !mblk_rm(blk) )
		return 0;
	Uint32 naddr = kdalloc_global(newsz,algn,phys);
	if ( !naddr )
		return 0;
	memmove((Uint8*)naddr,(Uint8*)prev,osz);
	return naddr;
}

/* vanilla */
Uint32 kdrealloc( Uint32 prev, Uint32 newsz )
{
	return kdrealloc_global(prev,newsz,0,NULL);
}

/* page-aligned */
Uint32 kdrealloc_a( Uint32 prev, Uint32 newsz )
{
	return kdrealloc_global(prev,newsz,1,NULL);
}

/* return physical address */
Uint32 kdrealloc_p( Uint32 prev, Uint32 newsz, Uint32 *phys )
{
	return kdrealloc_global(prev,newsz,0,phys);
}

/* page-aligned and return physical address */
Uint32 kdrealloc_ap( Uint32 prev, Uint32 newsz, Uint32 *phys )
{
	return kdrealloc_global(prev,newsz,1,phys);
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
		mem += kdmem_list[i].end-kdmem_list[i].start;
	return mem;
}

/* initialize dynamic memory allocator */
void kdmem_init( Uint32 start, Uint32 size, Uint32 psize )
{
	/* sanity checks */
	if ( !size )
		BERP("Zero allication area");
	if ( psize < sizeof(memblk_t) )
		BERP("Pile size is too small");
	kdmem_range[0] = start+psize;
	kdmem_range[1] = start+psize+size;
	/* list */
	kdmem_psize_max = psize/sizeof(memblk_t);
	/* list is located at the start, followed by the memory area */
	kdmem_list = (memblk_t*)start;
	/* will not check if there is enough space available for it */
	memset((Uint8*)kdmem_list,0,kdmem_psize_max*sizeof(memblk_t));
	kdmem_psize = 0;
	kdmem_enabled = 1;
}
