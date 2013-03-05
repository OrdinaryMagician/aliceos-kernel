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
extern Uint32 p_addr;
extern Uint32 m_addr;

/* directories */
page_directory_t *current_directory = NULL;
page_directory_t *kernel_directory = NULL;

/* bitset of frames */
Uint32 *frames;
Uint32 nframes;

/* prototypes */
static void page_fault( regs_t *regs );

/* set a bit in the bitset */
static void set_frame( Uint32 addr )
{
	Uint32 faddr = addr/0x1000;
	Uint32 idx = faddr/0x20;
	Uint32 off = faddr%0x20;
	frames[idx] |= 1<<off;
}

/* clear a bit in the bitset */
static void clr_frame( Uint32 addr )
{
	Uint32 faddr = addr/0x1000;
	Uint32 idx = faddr/0x20;
	Uint32 off = faddr%0x20;
	frames[idx] &= ~(1<<off);
}

/* test the value of a bit in the bitset */
static Uint32 chk_frame( Uint32 addr )
{
	Uint32 faddr = addr/0x1000;
	Uint32 idx = faddr/0x20;
	Uint32 off = faddr%0x20;
	return frames[idx]&(1<<off);
}

/* find the first free frame */
/* return UINT32_MAX if there are no free frames */
static Uint32 ffree_frame( void )
{
	Uint32 i, j;
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
void alloc_frame( page_t *page, Uint8 iskernel, Uint8 iswriteable )
{
	if ( page->frame )
		return;
	Uint32 idx = ffree_frame();
	if ( idx == UINT32_MAX )
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
	Uint32 frm = page->frame;
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
	frames = (Uint32*)kmalloc(nframes/0x20);
	memset((Uint8*)frames,0,nframes/0x20);
	/* a page directory for the kernel */
	kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
	memset((Uint8*)kernel_directory,0,sizeof(page_directory_t));
	current_directory = kernel_directory;
	Uint32 i;
	/* dynamic allocator frames, pass 1 */
	for ( i=KDMEM_ST; i<KDMEM_ST+KDMEM_SIZ+KDMEM_RESV; i+=0x1000 )
		get_page(i,1,kernel_directory);
	/* kernel frames */
	for ( i=0; i<p_addr; i+=0x1000 )
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
extern void loadcr3( Uint32 phys );
extern void enablepaging( void );
extern Uint32 getfaultaddr( void );

/* loads the specified page directory in CR3 */
void switch_pdir( page_directory_t *newdir )
{
	current_directory = newdir;
	loadcr3((Uint32)&newdir->tablesPhysical);
	enablepaging();
}

/* get a specific page, if make isn't zero, create it if it doesn't exist */
page_t *get_page( Uint32 addr, Uint8 make, page_directory_t *dir )
{
	addr /= 0x1000;
	Uint32 tidx = addr/0x400;
	if ( dir->tables[tidx] )
		return &dir->tables[tidx]->pages[addr%0x400];
	if ( make )
	{
		Uint32 tmp;
		dir->tables[tidx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t),&tmp);
		memset((Uint8*)dir->tables[tidx],0,sizeof(page_table_t));
		dir->tablesPhysical[tidx] = tmp|7;	/* present, rw, user */
		return &dir->tables[tidx]->pages[addr%0x400];
	}
	return 0;
}

/* slightly customized page fault handler */
static void page_fault( regs_t *regs )
{
	/* save registers just in case */
	regs_t srg;
	memcpy((Uint8*)&srg,(Uint8*)regs,sizeof(regs_t));
	/* get the faulting address */
	Uint32 fault_addr = getfaultaddr();
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
	Uint32 val = fault_addr;
	char digs[8];
	Sint8 i = 0;
	do
	{
		digs[i++] = ((val&0x0F)>0x09)?('A'+(val&0x0F)-0x0A):('0'+(val&0x0F));
		val >>= 4;
	}
	while ( (val != 0) && (i < 8) );
	while ( i < 8 )
		digs[i++] = '0';
	while ( i > 0 )
		strncat(pfmesg,&digs[--i],1);
	OHSHI(pfmesg,&srg);
}
