/*
	ramdisk.c : Functions for handling the ramdisk.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <fs/ramdisk.h>
#include <memops.h>
#include <strops.h>
#include <printk.h>
#include <berp.h>

Uint32 aosrd_hdmagic = 0xFEEDCAFE;
Uint32 aosrd_trmagic = 0xADEADBED;
char aosrd_trsig[28] = "The Doll Maker of Bucuresti";

Uint32 rdpos = 0;
Uint32 rdsiz = 0;

/* try to find an entry in the ramdisk by filename */
rd_entry_t *rd_find( char *fname )
{
	if ( !rdpos )
	{
		printk("Warning: Attempt to access uninitialized ramdisk\n");
		return NULL;
	}
	Uint32 ents = rd_numents();
	Uint32 i;
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
Uint32 rd_find_data( char *fname )
{
	rd_entry_t *found = rd_find(fname);
	if ( found == NULL )
		return 0;
	return (found->start+rdpos);
}

/* retrieve an entry by its index number */
rd_entry_t *rd_entry( Uint32 idx )
{
	if ( !rdpos )
	{
		printk("Warning: Attempt to access uninitialized ramdisk\n");
		return NULL;
	}
	if ( idx >= rd_numents() )
	{
		printk("Warning: Attempting to access ramdisk entry out of bounds\n");
		return NULL;
	}
	return (rd_entry_t*)(rdpos+RDHEAD_SIZ+(RDENT_SIZ*idx));
}

/* retrive the number of entries in the ramdisk */
Uint32 rd_numents( void )
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
Uint8 init_ramdisk( Uint32 start, Uint32 end )
{
	rdpos = start;
	rdsiz = end-start;
	printk("Setting up ramdisk at %#08x (%u bytes)... ",rdpos,rdsiz);
	/* verify disk */
	/* header */
	Uint32 mag = *(Uint32*)rdpos;
	if ( mag != aosrd_hdmagic )
	{
		printk("\033[1;31mfailed\n Header invalid\n");
		printk("  expected 0x%08x\n",aosrd_hdmagic);
		printk("  got 0x%08x\033[0m\n",mag);
		rdpos = 0;
		rdsiz = 0;
		return 1;
	}
	/* footer (trailer) */
	Uint32 fmg = *(Uint32*)(rdpos+rdsiz-32);
	char fsig[28];
	strncpy(fsig,(char*)(rdpos+rdsiz-28),27);
	if ( fmg != aosrd_trmagic )
	{
		printk("\033[1;31mfailed\n Trailer magic invalid\n");
		printk("  expected 0x%08x\n",aosrd_trmagic);
		printk("  got 0x%08x[0m\n",fmg);
		rdpos = 0;
		rdsiz = 0;
		return 1;
	}
	if ( strncmp(fsig,aosrd_trsig,28) )
	{
		printk("\033[1;31mfailed\n Trailer signature invalid\n");
		printk("  expected \"%s\"\n",aosrd_trsig);
		printk("  got \"%s\"\033[0m\n",fsig);
		rdpos = 0;
		rdsiz = 0;
		return 1;
	}
	printk("\033[1;32mok\033[0m\n");
	return 0;
}
