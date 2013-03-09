/*
	paging.h : Paging (duh).
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef PAGING_H
#define PAGING_H
#include <sys/types.h>
/*
	Page structure

	32 bits

	0     Present
	1     Read/Write
	2     User/Supervisor
	3-4   Reserved
	5     Accessed
	6     Dirty (Written to)
	7-8   Reserved
	9-11  Reserved for CPU use
	12-31 Frame address
*/
typedef struct
{
	/* Bitfields are phun~ */
	Uint32 present  : 1;
	Uint32 rw       : 1;
	Uint32 user     : 1;
	Uint32          : 2;
	Uint32 accessed : 1;
	Uint32 dirty    : 1;
	Uint32          : 2;
	Uint32          : 3;
	Uint32 frame    :20;
} page_t;
/* 4KB page table */
typedef struct
{
	page_t pages[1024];
} ptbl_t;
/* page directory */
typedef struct
{
	/* pointers to each table */
	ptbl_t *tables[1024];
	/* physical addresses of each table */
	Uint32 tablesPhysical[1024];
	/* the physical address of the array above */
	Uint32 physAddr;
} pdir_t;
/* allocate a frame */
void alloc_frame( page_t *page, Uint8 iskernel, Uint8 iswriteable );
/* free a frame */
void free_frame( page_t *page );
/* initialize paging */
void init_paging( void );
/* loads the specified page directory in CR3 */
void switch_pdir( pdir_t *newdir );
/* get a specific page, if make isn't zero, create it if it doesn't exist */
page_t *get_page( Uint32 addr, Uint8 make, pdir_t *dir );
#endif
