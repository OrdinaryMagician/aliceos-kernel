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

	The used memory is managed in a pile of blocks.

	Every time there's an allocation, the manager checks
	if there's any unused gaps inside kdmem_range where the
	requested amount of space can fit. The new block is
	stacked onto the pile and becomes used memory. If there's
	no space available the allocation function returns 0.

	Every time a memory block is freed, said block is deleted
	and any blocks above it are pushed down.
	
	After some time, the pile will end up with blocks that aren't
	in order. When this happens, the allocator might work slowly (or not).
	It's recommended to schedule a task to call kdmem_tidy() every
	once in a while, to rearrange the pile.
	
	This model is ridiculously inefficient, but hell, at least it's
	easy to implement.
	
	It slows down considerably after a thousand allocations or more,
	but I plan to fix that. I just need to optimize the code for the
	block finders (firstblk,prevblk,nextblk,lastblk).
*/

/* memory block pile */
static memblk_t *kdmem_pile = NULL;
/* total (virtual) memory range available for allocation */
static memblk_t kdmem_range = {0x00000000,0xFFFFFFFF};
/* pile sizes */
static Uint32 kdmem_psize = 0;
static Uint32 kdmem_psize_max = 0;

/* compare blocks */
/* 0: equal, 1: below, 2: above, 4: overlap */
static Uint8 blkcmp( memblk_t *a, memblk_t *b )
{
	if ( (a->start == b->start) && (a->end == b->end) )
		return 0;
	if ( (a->start < b->start) && (a->end < b->start) )
		return 1;
	if ( (a->start > b->start) && (a->start > b->end) )
		return 2;
	return 4;
}

/* calculate distance between blocks (must not overlap) */
static Uint32 blkdist( memblk_t *a, memblk_t *b )
{
	Uint8 compared = blkcmp(a,b);
	if ( compared&1 )
		return (b->start-a->end);
	if ( compared&2 )
		return (a->start-b->end);
	return 0; /* equal or overlapping */
}

/* find closest block above current in a possibly unordered array */
/* return NULL if not found */
static memblk_t* nextblk( memblk_t *cur )
{
	memblk_t *last = NULL;
	Uint32 lastdist = UINT32_MAX;
	Uint32 dist;
	Uint32 i;
	for ( i=0; i<kdmem_psize; i++ )
	{
		dist = blkdist(cur,&kdmem_pile[i]);
		if ( (blkcmp(cur,&kdmem_pile[i])&2) && (dist<lastdist) )
		{
			last = &kdmem_pile[i];
			lastdist = dist;
		}
	}
	return last;
}

/* find closest block below current in a possibly unordered array */
/* return NULL if not found */
static memblk_t* prevblk( memblk_t *cur )
{
	memblk_t *last = NULL;
	Uint32 lastdist = UINT32_MAX;
	Uint32 dist;
	Uint32 i;
	for ( i=0; i<kdmem_psize; i++ )
	{
		dist = blkdist(cur,&kdmem_pile[i]);
		if ( (blkcmp(cur,&kdmem_pile[i])&1) && (dist<lastdist) )
		{
			last = &kdmem_pile[i];
			lastdist = dist;
		}
	}
	return last;
}

/* find the lowest block in a possibly unordered array */
/* return NULL if not found (shouldn't happen) */
static memblk_t* firstblk( void )
{
	memblk_t *last = NULL;
	Uint32 lastdist = UINT32_MAX;
	Uint32 dist;
	Uint32 i;
	for ( i=0; i<kdmem_psize; i++ )
	{
		dist = kdmem_pile[i].start-kdmem_range.start;
		if ( dist<lastdist )
		{
			last = &kdmem_pile[i];
			lastdist = dist;
		}
	}
	return last;
}

/* find the highest block in a possibly unordered array */
/* return NULL if not found (shouldn't happen) */
static memblk_t* lastblk( void )
{
	memblk_t *last = NULL;
	Uint32 lastdist = UINT32_MAX;
	Uint32 dist;
	Uint32 i;
	for ( i=0; i<kdmem_psize; i++ )
	{
		dist = kdmem_range.end-kdmem_pile[i].end;
		if ( dist<lastdist )
		{
			last = &kdmem_pile[i];
			lastdist = dist;
		}
	}
	return last;
}

/* find the first gap where we can fit a specific block */
static Uint32 mblk_findgap( Uint32 sz, Uint8 algn )
{
	/* start from first block, check the space in the next block */
	/* if it fits, return address, if not, return 0 */
	memblk_t *blk_a = firstblk();
	if ( !blk_a ) /* no blocks allocated */
	{
		if ( sz < (kdmem_range.end-kdmem_range.start) ) /* it fits */
			return kdmem_range.start;
		return 0; /* too big */
	}
	memblk_t *blk_b = nextblk(blk_a);
	if ( !blk_b ) /* only one block allocated */
	{
		if ( sz < (kdmem_range.end-(blk_a->end+1)) ) /* it fits */
			return blk_a->end+1;
		return 0; /* too big */
	}
	Uint32 dist;
	while ( blk_b )
	{
		dist = blkdist(blk_a,blk_b);
		if ( dist > sz ) /* t-this hole... it was made for me! */
			return blk_a->end+1;
		blk_a = blk_b;
		blk_b = nextblk(blk_a);
	}
	return 0; /* nope, no match */
}

/* add a memory block to the pile */
static Uint8 mblk_add( Uint32 start, Uint32 end )
{
	/*
	   no need to check for overlaps, we assume the allocator function
	   called findgap first and it succeeded in giving a non-overlapping
	   area
	*/
	if ( kdmem_psize < kdmem_psize_max )
	{
		/* put it on the pile */
		kdmem_pile[kdmem_psize].start = start;
		kdmem_pile[kdmem_psize].end = end;
		kdmem_psize++;
		return 1;
	}
	return 0;
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
	memset((Uint8*)(kdmem_pile+idx),0,sizeof(memblk_t));
	kdmem_psize--;
	if ( idx < kdmem_psize ) /* push back anything on top if needed */
		memmove((Uint8*)(kdmem_pile+idx+1),(Uint8*)(kdmem_pile+idx),
			sizeof(memblk_t)*(kdmem_psize-idx));
	return 1;
}

/* find the memory block that starts at a specific address */
static Uint32 mblk_find( Uint32 addr )
{
	Uint32 i;
	for ( i=0; i<kdmem_psize; i++ )
		if ( kdmem_pile[i].start == addr )
			return i;
	return UINT32_MAX;
}

/* reserve a memory area */
Uint32 kdalloc_int( Uint32 sz, Uint8 algn, Uint32 *phys ) /* internal */
{
	Uint32 addr = mblk_findgap(sz,algn);
	if ( !addr )
		return 0; /* no space available */
	if ( !mblk_add(addr,addr+sz) )
		return 0; /* couldn't add block */
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

/* vanilla */
Uint32 kdalloc( Uint32 sz )
{
	return kdalloc_int(sz,0,NULL);
}

/* page-aligned */
Uint32 kdalloc_a( Uint32 sz )
{
	return kdalloc_int(sz,1,NULL);
}

/* return physical address */
Uint32 kdalloc_p( Uint32 sz, Uint32 *phys )
{
	return kdalloc_int(sz,0,phys);
}

/* page-aligned and return physical address */
Uint32 kdalloc_ap( Uint32 sz, Uint32 *phys )
{
	return kdalloc_int(sz,1,phys);
}

/* reallocate (resize) a memory area */
Uint32 kdrealloc_int( Uint32 prev, Uint32 newsz, Uint8 algn, Uint32 *phys )
{
	/* TODO */
	BERP("Not implemented");
	return 0;
}

/* vanilla */
Uint32 kdrealloc( Uint32 prev, Uint32 newsz )
{
	return kdrealloc_int(prev,newsz,0,NULL);
}

/* page-aligned */
Uint32 kdrealloc_a( Uint32 prev, Uint32 newsz )
{
	return kdrealloc_int(prev,newsz,1,NULL);
}

/* return physical address */
Uint32 kdrealloc_p( Uint32 prev, Uint32 newsz, Uint32 *phys )
{
	return kdrealloc_int(prev,newsz,0,phys);
}

/* page-aligned and return physical address */
Uint32 kdrealloc_ap( Uint32 prev, Uint32 newsz, Uint32 *phys )
{
	return kdrealloc_int(prev,newsz,1,phys);
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
	if ( end <= start )
		BERP("Zero or negative allication area");
	if ( psize < sizeof(memblk_t) )
		BERP("Pile size is too small");
	kdmem_range.start = start;
	kdmem_range.end = end;
	/* pile */
	kdmem_psize_max = psize/sizeof(memblk_t);
	/* pile is located right at the end */
	kdmem_pile = (memblk_t*)end;
	/* will not check if there is enough space available for it */
	memset((Uint8*)kdmem_pile,0,kdmem_psize_max*sizeof(memblk_t));
	kdmem_psize = 0;
}
