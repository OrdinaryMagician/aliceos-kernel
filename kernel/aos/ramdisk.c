/*
	ramdisk.c : Functions for handling the ramdisk.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <ramdisk.h>

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
		printk_s(SERIAL_A,"Warning: Attempt to access uninitialized ramdisk\n");
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
		printk_s(SERIAL_A,"Warning: Attempt to access uninitialized ramdisk\n");
		return NULL;
	}
	if ( idx >= rd_numents() )
	{
		printk_s(SERIAL_A,"Warning: Attempting to access ramdisk entry out of bounds\n");
		return NULL;
	}
	return (rd_entry_t*)(rdpos+RDHEAD_SIZ+(RDENT_SIZ*idx));
}

/* retrive the number of entries in the ramdisk */
Uint32 rd_numents( void )
{
	if ( !rdpos )
	{
		printk_s(SERIAL_A,"Warning: Attempt to access uninitialized ramdisk\n");
		return 0;
	}
	rd_header_t *head = (rd_header_t*)rdpos;
	return head->numents;
}

/* initialize ramdisk handler */
void init_ramdisk( Uint32 start, Uint32 end )
{
	rdpos = start;
	rdsiz = end-start;
	printk_s(SERIAL_A,"Ramdisk at %#08x (%u bytes)\n",rdpos,rdsiz);
	/* verify disk */
	/* header */
	Uint8 mag[4];
	memcpy(&mag[0],(Uint8*)rdpos,4);
	if ( memcmp(&mag[0],&aosrd_hdmagic[0],4) )
	{
		printk_s(SERIAL_A,"Expected magic: 0x%02x%02x%02x%02x\n",aosrd_hdmagic[0],aosrd_hdmagic[1],aosrd_hdmagic[2],aosrd_hdmagic[3]);
		printk_s(SERIAL_A,"Ramdisk magic:  0x%02x%02x%02x%02x\n",mag[0],mag[1],mag[2],mag[3]);
		BERP("Ramdisk failed format checks, incorrect header magic");
	}
	/* footer (trailer) */
	Uint8 fmg[4];
	memcpy(&fmg[0],(Uint8*)(rdpos+rdsiz-32),4);
	Uint8 fsig[28];
	memcpy(&fsig[0],(Uint8*)(rdpos+rdsiz-28),28);
	if ( memcmp(&fmg[0],&aosrd_trmagic[0],4) )
	{
		printk_s(SERIAL_A,"Expected magic: 0x%02x%02x%02x%02x\n",aosrd_trmagic[0],aosrd_trmagic[1],aosrd_trmagic[2],aosrd_trmagic[3]);
		printk_s(SERIAL_A,"Ramdisk magic:  0x%02x%02x%02x%02x\n",fmg[0],fmg[1],fmg[2],fmg[3]);
		BERP("Ramdisk failed format checks, incorrect trailer magic");
	}
	if ( memcmp((Uint8*)&fsig[0],(Uint8*)&aosrd_trsig[0],28) )
	{
		printk_s(SERIAL_A,"Expected signature: %s",aosrd_trsig);
		printk_s(SERIAL_A,"Ramdisk signature:  %s",fsig);
		BERP("Ramdisk failed format checks, incorrect trailer signature");
	}
}
