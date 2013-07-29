/*
	ramdisk.c : Functions for handling the ramdisk.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/types.h>
#include <fs/ramdisk.h>
#include <printk.h>
#include <strops.h>
uint32_t rdpos = 0;
uint32_t rdsiz = 0;
/* try to find an entry in the ramdisk by filename */
rd_entry_t *rd_find( char *fname )
{
	if ( !rdpos )
	{
		printk("Warning: Attempt to access uninitialized ramdisk\n");
		return NULL;
	}
	uint32_t ents = rd_numents();
	uint32_t i;
	rd_entry_t *ent;
	for ( i=0; i<ents; i++ )
	{
		ent = (rd_entry_t*)(rdpos+RDHEAD_SIZ+(RDENT_SIZ*i));
		if ( !strcmp(ent->name,fname) )
			return ent;
	}
	/* not found */
	return NULL;
}
/* try to find an entry in the ramdisk by filename, return its data address */
uint32_t rd_find_data( char *fname )
{
	rd_entry_t *found = rd_find(fname);
	if ( found == NULL )
		return 0;
	return (found->start+rdpos);
}
/* retrieve an entry by its index number */
rd_entry_t *rd_entry( uint32_t idx )
{
	if ( !rdpos )
	{
		printk("Warning: Attempt to access uninitialized ramdisk\n");
		return NULL;
	}
	if ( idx >= rd_numents() )
	{
		printk("Warning: Attempting to access entry out of bounds\n");
		return NULL;
	}
	return (rd_entry_t*)(rdpos+RDHEAD_SIZ+(RDENT_SIZ*idx));
}
/* retrive the number of entries in the ramdisk */
uint32_t rd_numents( void )
{
	if ( !rdpos )
	{
		printk("Warning: Attempt to access uninitialized ramdisk\n");
		return 0;
	}
	rd_header_t *head = (rd_header_t*)rdpos;
	return head->numents;
}
/* initialize ramdisk handler */
uint8_t init_ramdisk( uint32_t start, uint32_t end )
{
	rdpos = start;
	rdsiz = end-start;
	printk("Setting up ramdisk at %#08x (%u bytes)... ",rdpos,rdsiz);
	/* verify disk */
	/* header */
	uint32_t mag = *(uint32_t*)rdpos;
	if ( mag != AOSRD_HDMAGIC )
	{
		printk("\033[1;31mfailed\n Header invalid\n");
		printk("  expected 0x%08x\n",AOSRD_HDMAGIC);
		printk("  got 0x%08x\033[0m\n",mag);
		rdpos = 0;
		rdsiz = 0;
		return 1;
	}
	/* footer (trailer) */
	uint32_t fmg = *(uint32_t*)(rdpos+rdsiz-32);
	char fsig[28];
	strncpy(fsig,(char*)(rdpos+rdsiz-28),27);
	if ( fmg != AOSRD_TRMAGIC )
	{
		printk("\033[1;31mfailed\n Trailer magic invalid\n");
		printk("  expected 0x%08x\n",AOSRD_TRMAGIC);
		printk("  got 0x%08x\033[0m\n",fmg);
		rdpos = 0;
		rdsiz = 0;
		return 1;
	}
	if ( strncmp(fsig,AOSRD_TRSIG,28) )
	{
		printk("\033[1;31mfailed\n Trailer signature invalid\n");
		printk("  expected \"%s\"\n",AOSRD_TRSIG);
		printk("  got \"%s\"\033[0m\n",fsig);
		rdpos = 0;
		rdsiz = 0;
		return 1;
	}
	printk("\033[1;32mok\033[0m\n");
	return 0;
}
