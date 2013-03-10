/*
	paging.c : Paging (duh).
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/helpers.h>
#include <sys/paging.h>
#include <sys/isr_handlers.h>
#include <sys/regs.h>
#include <hcf.h>
#include <berp.h>
#include <kmem.h>
#include <kdmem.h>
#include <strops.h>
#include <memops.h>
#include <printk.h>
/* from kmem.c */
extern uint32_t p_addr;
extern uint32_t m_addr;
/* directories */
pdir_t *current_directory = NULL;
pdir_t *kernel_directory = NULL;
/* bitset of frames */
uint32_t *frames;
uint32_t nframes;
/* prototypes */
static void page_fault( regs_t *regs );
/* set a bit in the bitset */
static void set_frame( uint32_t addr )
{
	uint32_t faddr = addr/0x1000;
	uint32_t idx = faddr/0x20;
	uint32_t off = faddr%0x20;
	frames[idx] |= 1<<off;
}
/* clear a bit in the bitset */
static void clr_frame( uint32_t addr )
{
	uint32_t faddr = addr/0x1000;
	uint32_t idx = faddr/0x20;
	uint32_t off = faddr%0x20;
	frames[idx] &= ~(1<<off);
}
/* test the value of a bit in the bitset */
static uint32_t chk_frame( uint32_t addr )
{
	uint32_t faddr = addr/0x1000;
	uint32_t idx = faddr/0x20;
	uint32_t off = faddr%0x20;
	return frames[idx]&(1<<off);
}
/* find the first free frame, return UINT32_MAX if there are no free frames */
static uint32_t ffree_frame( void )
{
	uint32_t i, j;
	for ( i=0; i<(nframes/0x20); i++ )
	{
		if ( frames[i] == 0xFFFFFFFF )
			continue;
		for ( j=0; j<32; j++ )
			if ( !(frames[i]&(1<<j)) )
				return i*0x20+j;
	}
	return UINT32_MAX;
}
/* allocate a frame */
void alloc_frame( page_t *page, uint8_t iskernel, uint8_t iswriteable )
{
	if ( page->frame )
		return;
	uint32_t idx = ffree_frame();
	if ( idx == UINT32_MAX ) /* we're screwed */
		BERP("No more free frames");
	/* take it */
	set_frame(idx*0x1000);
	page->present = 1;
	page->rw = iswriteable?1:0;
	page->user = iskernel?0:1;
	page->frame = idx;
}
/* free a frame */
void free_frame( page_t *page )
{
	uint32_t frm = page->frame;
	if ( !frm )
		return;	/* no need to do anything */
	/* clear 'n stuff */
	clr_frame(frm*0x1000);
	page->frame = 0;
}
/* initialize paging */
void init_paging( void )
{
	nframes = m_addr/0x1000;
	frames = kmalloc(nframes/0x20);
	memset((uint8_t*)frames,0,nframes/0x20);
	/* a page directory for the kernel */
	kernel_directory = kmalloc_a(sizeof(pdir_t));
	memset((uint8_t*)kernel_directory,0,sizeof(pdir_t));
	current_directory = kernel_directory;
	uint32_t i;
	/* dynamic allocator frames, pass 1 */
	for ( i=KDMEM_ST; i<KDMEM_ST+KDMEM_SIZ+KDMEM_RESV; i+=0x1000 )
		get_page(i,1,kernel_directory);
	/* kernel frames */
	for ( i=0; i<p_addr+0x3000; i+=0x1000 )
		alloc_frame(get_page(i,1,kernel_directory),0,0);
	/* dynamic allocator frames, pass 2 */
	for ( i=KDMEM_ST; i<KDMEM_ST+KDMEM_SIZ+KDMEM_RESV; i+=0x1000 )
		alloc_frame(get_page(i,0,kernel_directory),0,0);
	/* register our custom page fault handler */
	register_isr_handler(14,page_fault);
	/* finally enable paging */
	switch_pdir(kernel_directory);
	/* initialize the dynamic allocator */
	kdmem_init(KDMEM_ST,KDMEM_SIZ,KDMEM_RESV);
}
/* these functions are in pagingasm.s */
extern void loadcr3( uint32_t phys );
extern void enablepaging( void );
extern uint32_t getfaultaddr( void );
/* loads the specified page directory in CR3 */
void switch_pdir( pdir_t *newdir )
{
	current_directory = newdir;
	loadcr3((uint32_t)&newdir->tablesPhysical);
	enablepaging();
}
/* get a specific page, if make isn't zero, create it if it doesn't exist */
page_t *get_page( uint32_t addr, uint8_t make, pdir_t *dir )
{
	addr /= 0x1000;
	uint32_t tidx = addr/0x400;
	if ( dir->tables[tidx] )
		return &dir->tables[tidx]->pages[addr%0x400];
	if ( make )
	{
		uint32_t tmp;
		dir->tables[tidx] = kmalloc_ap(sizeof(ptbl_t),&tmp);
		memset((uint8_t*)dir->tables[tidx],0,sizeof(ptbl_t));
		dir->tablesPhysical[tidx] = tmp|7; /* present, rw, user */
		return &dir->tables[tidx]->pages[addr%0x400];
	}
	return 0;
}
/* slightly customized page fault handler */
static void page_fault( regs_t *regs )
{
	/* save registers just in case */
	regs_t srg;
	memcpy((uint8_t*)&srg,(uint8_t*)regs,sizeof(regs_t));
	/* get the faulting address */
	uint32_t fault_addr = getfaultaddr();
	char pfmesg[256];
	strcpy(pfmesg,"SEGMENTATION FAULT");
	if ( !(srg.errno&1) )
		strcat(pfmesg," [P]");
	if ( srg.errno&2 )
		strcat(pfmesg," [RO]");
	if ( srg.errno&4 )
		strcat(pfmesg," [USR]");
	if ( srg.errno&8 )
		strcat(pfmesg," [RESV]");
	if ( srg.errno&10 )
		strcat(pfmesg," [INST]");
	strcat(pfmesg," AT 0x");
	uint32_t val = fault_addr;
	char digs[8];
	int8_t i = 0;
	do
	{
		digs[i++] = ((val&0x0F)>0x09)?('A'+(val&0x0F)-0x0A)
				:('0'+(val&0x0F));
		val >>= 4;
	}
	while ( (val != 0) && (i < 8) );
	while ( i < 8 )
		digs[i++] = '0';
	while ( i > 0 )
		strncat(pfmesg,&digs[--i],1);
	OHSHI(pfmesg,&srg);
}
