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
#include <printk.h>
/*
The used memory is managed in a list of blocks.
Every time there's an allocation, the manager checks if there's any unused gaps
inside kdmem_range where the requested amount of space can fit.
The new block is inserted into the list and becomes used memory.
If the new block is going to be inserted between already existing entries,
everything above its new index is pushed forward.
If there's no space available the allocation function returns 0.
Every time a memory block is freed, said block is deleted and any blocks above
it are pushed down.
This is probably not every efficient. It slows down a lot after thousands of
allocations (exponentially, even).
Expanding and shrinking of the memory area not implemented yet.
*/
/* in sys/paging.c */
extern pdir_t *kernel_directory;
/* in sys/pagingasm.s */
extern uint8_t pagingenabled( void );
/* memory block list */
static memblk_t *kdmem_list = NULL;
/* total (virtual) memory range available for allocation */
static uint32_t kdmem_range[2] = {0x00000000,0xFFFFFFFF};
/* pile sizes */
static uint32_t kdmem_psize = 0;
static uint32_t kdmem_psize_max = 0;
/* we're enabled? */
uint8_t kdmem_enabled = 0;
/* find the first gap where we can fit a specific block */
static uint32_t mblk_findgap( uint32_t sz, uint8_t algn, uint32_t *idx )
{
	uint32_t i;
	uint32_t pa = kdmem_range[0], pb = kdmem_range[1];
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
static uint8_t mblk_add( uint32_t idx, uint32_t start, uint32_t end )
{
	/* list full */
	if ( kdmem_psize >= kdmem_psize_max )
		return 0;
	if ( idx < kdmem_psize ) /* push everything above */
		memmove(&kdmem_list[idx+1],
			&kdmem_list[idx],
			sizeof(memblk_t)*(kdmem_psize-idx));
	/* put it on the list */
	kdmem_list[idx].start = start;
	kdmem_list[idx].end = end;
	kdmem_psize++;
	return 1;
}
/* remove a memory block from the pile */
static uint8_t mblk_rm( uint32_t idx )
{
	/* if there are no blocks reserved yet */
	if ( !kdmem_psize )
		return 0;
	/* if we're trying to go beyond index */
	if ( idx >= kdmem_psize )
		return 0;
	/* zero out this block entry */
	memset(&kdmem_list[idx],0,sizeof(memblk_t));
	if ( idx < --kdmem_psize ) /* push back anything on top if needed */
		memmove(&kdmem_list[idx],
			&kdmem_list[idx+1],
			sizeof(memblk_t)*(kdmem_psize-idx));
	return 1;
}
/* find the memory block that starts at a specific address */
static uint32_t mblk_find( uint32_t addr )
{
	uint32_t i;
	for ( i=0; i<kdmem_psize; i++ )
		if ( kdmem_list[i].start == addr )
			return i;
	return UINT32_MAX;
}
/* reserve a memory area */
/* global */
void *kdalloc_global( uint32_t sz, uint8_t alg, uint32_t *phys )
{
	uint32_t idx = 0;
	uint32_t addr = mblk_findgap(sz,alg,&idx);
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
	return (void*)addr;
}
/* vanilla */
void *kdalloc( uint32_t sz )
{
	return kdalloc_global(sz,0,NULL);
}
/* page-aligned */
void *kdalloc_a( uint32_t sz )
{
	return kdalloc_global(sz,1,NULL);
}
/* return physical address */
void *kdalloc_p( uint32_t sz, uint32_t *phys )
{
	return kdalloc_global(sz,0,phys);
}
/* page-aligned and return physical address */
void *kdalloc_ap( uint32_t sz, uint32_t *phys )
{
	return kdalloc_global(sz,1,phys);
}
/* reallocate (resize) a memory area */
/* global */
void *kdrealloc_global( void *prev, uint32_t sz, uint8_t alg, uint32_t *phys )
{
	uint32_t blk = mblk_find((uint32_t)prev);
	if ( blk == UINT32_MAX )
		return 0; /* no block at this address */
	uint32_t osz = kdmem_list[blk].end-kdmem_list[blk].start;
	if ( !mblk_rm(blk) )
		return 0;
	void *naddr = kdalloc_global(sz,alg,phys);
	if ( !naddr )
		return 0;
	memmove(naddr,prev,osz);
	return naddr;
}
/* vanilla */
void *kdrealloc( void *prev, uint32_t sz )
{
	return kdrealloc_global(prev,sz,0,NULL);
}
/* page-aligned */
void *kdrealloc_a( void *prev, uint32_t sz )
{
	return kdrealloc_global(prev,sz,1,NULL);
}
/* return physical address */
void *kdrealloc_p( void *prev, uint32_t sz, uint32_t *phys )
{
	return kdrealloc_global(prev,sz,0,phys);
}
/* page-aligned and return physical address */
void *kdrealloc_ap( void *prev, uint32_t sz, uint32_t *phys )
{
	return kdrealloc_global(prev,sz,1,phys);
}
/* free a memory area */
void kdfree( void *a )
{
	uint32_t blk = mblk_find((uint32_t)a);
	if ( blk == UINT32_MAX )
		return; /* just ignore everything */
	mblk_rm(blk);
}
/* retrieve used blocks */
uint32_t kdmem_count( void )
{
	return kdmem_psize;
}
/* retrieve total memory used */
uint32_t kdmem_amount( void )
{
	uint32_t i;
	uint32_t mem = 0;
	for ( i=0; i<kdmem_psize; i++ )
		mem += kdmem_list[i].end-kdmem_list[i].start;
	return mem;
}
/* initialize dynamic memory allocator */
void kdmem_init( uint32_t start, uint32_t size, uint32_t psize )
{
	/* sanity checks */
	printk("Initializing dynamic allocator\n");
	if ( !size )
		BERP("Zero allocation area");
	if ( psize < sizeof(memblk_t) )
		BERP("Pile size is too small");
	kdmem_range[0] = start+psize;
	kdmem_range[1] = start+psize+size;
	printk(" Memory range: %#08x-%#08x (%u bytes)\n",
		kdmem_range[0],kdmem_range[1],kdmem_range[1]-kdmem_range[0]);
	/* list */
	kdmem_psize_max = psize/sizeof(memblk_t);
	/* list is located at the start, followed by the memory area */
	kdmem_list = (memblk_t*)start;
	/* will not check if there is enough space available for it */
	memset(kdmem_list,0,kdmem_psize_max*sizeof(memblk_t));
	printk(" Block list at %#08x, %u entries max\n",kdmem_list,
		kdmem_psize_max);
	kdmem_psize = 0;
	kdmem_enabled = 1;
	printk(" Dynamic memory management is enabled\n");
}
