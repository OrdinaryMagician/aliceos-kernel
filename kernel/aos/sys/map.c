/*
	map.c : Symbol map functions.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.	
*/
#include <sys/types.h>
#include <sys/map.h>
#include <fs/ramdisk.h>
#include <printk.h>
#include <sys/helpers.h>
#include <kmem.h>
#include <memops.h>
/* the loaded table */
static sysmap_t sysmap = {0,0};
/* helper function */
static uint8_t chrtohex( char c )
{
	return ((c>='0')&&(c<='9'))?(c-'0')
		:((c>='a')&&(c<='f'))?((c-'a')+10)
		:((c>='A')&&(c<='F'))?((c-'A')+10)
		:0;
}
/* load up and interpret system.map */
static int32_t readsysmap( void )
{
	rd_entry_t *map = rd_find("system.map");
	if ( !map )
		return -1;
	/* read loop */
	char *ps = (char*)rd_find_data("system.map");
	char *pe = ps+map->size;
	/* pass 1, count lines and allocate table */
	printk(" Counting... ");
	char *p = ps;
	while ( p < pe )
		sysmap.nsyms += *(p++) == '\n';
	sysmap.sym = kmalloc(sizeof(mapsym_t)*sysmap.nsyms);
	if ( !sysmap.sym )
		return -2;
	/* pass 2, add symbols */
	p = ps;
	for ( uint32_t i=0; (i<sysmap.nsyms)&&(p<pe); i++ )
	{
		/* address */
		while ( *p != ' ' )
		{
			sysmap.sym[i].addr <<= 4;
			sysmap.sym[i].addr |= chrtohex(*(p++));
		}
		p++;
		/* type */
		sysmap.sym[i].type = *(p++);
		p++;
		/* name */
		uint16_t j = 0;
		while ( (*p != '\n') && (j<256) )
			sysmap.sym[i].name[j++] = *(p++);
		sysmap.sym[i].name[j] = '\0';
		p++;
	}
	return sysmap.nsyms;
}
/* load up map */
void init_sysmap( void )
{
	printk("Initializing symbol map\n");
	int32_t reslt = readsysmap();
	if ( reslt < 0 )
	{
		printk("(ERROR %u)\n",-reslt);
		return;
	}
	printk("%u symbols read\n",reslt);
}
/* return nearest map entry for an address */
int sym_addr( uint32_t addr, mapsym_t *out )
{
	if ( !out )
		return 1;
	for ( uint32_t i=0; i<sysmap.nsyms; i++ )
	{
		if ( (sysmap.sym[i].addr <= addr) && (i < (sysmap.nsyms-1)
			&& (sysmap.sym[i+1].addr > addr)) )
		{
			memcpy(out,&sysmap.sym[i],sizeof(mapsym_t));
			return 0;
		}
	}
	return 1;
}
