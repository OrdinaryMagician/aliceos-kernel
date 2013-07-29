/*
	paging.h : Paging (duh).
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
/*
	Page structure (32 bits)
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
	uint32_t present  : 1;
	uint32_t rw       : 1;
	uint32_t user     : 1;
	uint32_t          : 2;
	uint32_t accessed : 1;
	uint32_t dirty    : 1;
	uint32_t          : 2;
	uint32_t          : 3;
	uint32_t frame    :20;
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
	uint32_t tblPhys[1024];
	/* the physical address of the array above */
	uint32_t physAddr;
} pdir_t;
/* allocate a frame */
void alloc_frame( page_t *page, uint8_t iskernel, uint8_t iswriteable );
/* free a frame */
void free_frame( page_t *page );
/* initialize paging */
void init_paging( void );
/* loads the specified page directory in CR3 */
void switch_pdir( pdir_t *newdir );
/* get a specific page, if make isn't zero, create it if it doesn't exist */
page_t *get_page( uint32_t addr, uint8_t make, pdir_t *dir );
/* clone a page directory */
pdir_t *clone_directory( pdir_t *src );
