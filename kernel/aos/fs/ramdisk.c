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

Uint8 aosrd_hdmagic[4] = {0xFE,0xED,0xCA,0xFE};
Uint8 aosrd_trmagic[4] = {0xAD,0xEA,0xDB,0xED};
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
	Uint8 mag[4];
	memcpy(&mag[0],(Uint8*)rdpos,4);
	if ( memcmp(&mag[0],&aosrd_hdmagic[0],4) )
	{
		printk("\033[1;31mfailed\n Header invalid\n");
		printk("  expected 0x%02x%02x%02x%02x\n",aosrd_hdmagic[0],aosrd_hdmagic[1],aosrd_hdmagic[2],aosrd_hdmagic[3]);
		printk("  got 0x%02x%02x%02x%02x\033[0m\n",mag[0],mag[1],mag[2],mag[3]);
		rdpos = 0;
		rdsiz = 0;
		return 1;
	}
	/* footer (trailer) */
	Uint8 fmg[4];
	memcpy(&fmg[0],(Uint8*)(rdpos+rdsiz-32),4);
	Uint8 fsig[28];
	memcpy(&fsig[0],(Uint8*)(rdpos+rdsiz-28),27);
	if ( memcmp(&fmg[0],&aosrd_trmagic[0],4) )
	{
		printk("\033[1;31mfailed\n Trailer magic invalid\n");
		printk("  expected 0x%02x%02x%02x%02x\n",aosrd_trmagic[0],aosrd_trmagic[1],aosrd_trmagic[2],aosrd_trmagic[3]);
		printk("  got 0x%02x%02x%02x%02x\033[0m\n",fmg[0],fmg[1],fmg[2],fmg[3]);
		rdpos = 0;
		rdsiz = 0;
		return 1;
	}
	if ( memcmp((Uint8*)&fsig[0],(Uint8*)&aosrd_trsig[0],28) )
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
