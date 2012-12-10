#ifndef PAGING_H
#define PAGING_H
#include "types.h"
#include "isr.h"
typedef struct page
{
	Uint32 present  : 1;
	Uint32 rw       : 1;
	Uint32 user     : 1;
	Uint32 accessed : 1;
	Uint32 dirty    : 1;
	Uint32 unused   : 7;
	Uint32 frame    : 20;
} page_t;

typedef struct page_table
{
	page_t pages[1024];
} page_table_t;

typedef struct page_directory
{
	page_table_t *tables[1024];
	Uint32 tablesPhysical[1024];
	Uint32 physicalAddr;
} page_directory_t;

void init_paging( void );
void switch_page_directory( page_directory_t *new );
page_t *get_page( Uint32 address, Uint8 make, page_directory_t *dir );
void page_fault( registers_t regs );
#endif
