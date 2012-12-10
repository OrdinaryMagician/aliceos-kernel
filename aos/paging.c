/*
	paging.c : Paging (duh).
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include "paging.h"
#include "kheap.h"
#include "string.h"
#include "screen.h"
#include "panic.h"

page_directory_t *kernel_directory = 0;
page_directory_t *current_directory = 0;

Uint32 *frames;
Uint32 nframes;

extern Uint32 placement_address;

#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

static void set_frame( Uint32 frame_addr )
{
	Uint32 frame = frame_addr/0x1000;
	Uint32 idx = INDEX_FROM_BIT(frame);
	Uint32 off = OFFSET_FROM_BIT(frame);
	frames[idx] |= (0x01 << off);
}

static void clear_frame( Uint32 frame_addr )
{
	Uint32 frame = frame_addr/0x1000;
	Uint32 idx = INDEX_FROM_BIT(frame);
	Uint32 off = OFFSET_FROM_BIT(frame);
	frames[idx] &= ~(0x01 << off);
}

static Uint32 test_frame( Uint32 frame_addr )
{
	Uint32 frame = frame_addr/0x1000;
	Uint32 idx = INDEX_FROM_BIT(frame);
	Uint32 off = OFFSET_FROM_BIT(frame);
	return (frames[idx] & (0x01 << off));
}

static Uint32 first_frame( void )
{
	Uint32 i, j;
	for ( i=0; i<INDEX_FROM_BIT(nframes); i++ )
	{
		if ( frames[i] == 0xFFFFFFFF )
			continue;
		for ( j=0; j<32; j++ )
		{
			Uint32 toTest = 0x01 << j;
			if ( !frames[i]&toTest )
				return i*4*8+j;
		}
	}
}

void alloc_frame( page_t *page, Uint8 is_kernel, Uint8 is_writeable )
{
	if ( page->frame != 0 )
		return;
	Uint32 idx = first_frame();
	if ( idx == (Uint32)-1 )
		PANIC("No free frames!");
	set_frame(idx*0x1000);
	page->present = 1;
	page->rw = (is_writeable) ? 1 : 0;
	page->user = (is_kernel) ? 0 : 1;
	page->frame = idx;
}

void free_frame( page_t *page )
{
	Uint32 frame;
	if ( !(frame=page->frame) )
		return;
	clear_frame(frame);
	page->frame = 0x00;
}

void init_paging( void )
{
	Uint32 mem_end_page = 0x1000000;
	nframes = mem_end_page/0x1000;
	frames = (Uint32*)kmalloc(INDEX_FROM_BIT(nframes));
	memset(frames,0,INDEX_FROM_BIT(nframes));
	kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
	memset(kernel_directory,0,sizeof(page_directory_t));
	current_directory = kernel_directory;
	int i = 0;
	while ( i < placement_address )
	{
		alloc_frame(get_page(i,1,kernel_directory),0,0);
		i += 0x1000;
	}
	register_interrupt_handler(14,page_fault);
	switch_page_directory(kernel_directory);
}

void switch_page_directory( page_directory_t *dir )
{
	current_directory = dir;
	__asm__ __volatile__ ("mov %0, %%cr3"::"r"(&dir->tablesPhysical));
	Uint32 cr0;
	__asm__ __volatile__ ("mov %%cr0, %0":"=r"(cr0));
	cr0 |= 0x80000000;
	__asm__ __volatile__ ("mov %0, %%cr0"::"r"(cr0));
}

page_t *get_page( Uint32 address, Uint8 make, page_directory_t *dir )
{
	address /= 0x1000;
	Uint32 table_idx = address/1024;
	if ( dir->tables[table_idx] )
		return &dir->tables[table_idx]->pages[address%1024];
	if ( !make )
		return 0;
	Uint32 tmp;
	dir->tables[table_idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t),&tmp);
	memset(dir->tables[table_idx],0,0x1000);
	dir->tablesPhysical[table_idx] = tmp|0x07;
	return &dir->tables[table_idx]->pages[address%1024];
}

void page_fault( registers_t regs )
{
	Uint32 faulting_address;
	__asm__ __volatile__ ("mov %%cr2, %0":"=r"(faulting_address));
	Uint8 present = !(regs.err_code&0x01);
	Uint8 rw = regs.err_code&0x02;
	Uint8 us = regs.err_code&0x04;
	Uint8 reserved = regs.err_code&0x08;
	Uint8 id = regs.err_code&0x10;
	puts("page fault! ( ");
	if ( present )
		puts("present ");
	if ( rw )
		puts("read-only ");
	if ( us )
		puts("user-mode ");
	if ( reserved )
		puts("reserved ");
	puts(") at ");
	puth(faulting_address);
	putc('\n');
	PANIC("page fault");
}
