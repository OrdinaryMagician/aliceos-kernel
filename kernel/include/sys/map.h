/*
	map.h : Symbol map functions.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.	
*/
/* symbol map element */
typedef struct
{
	uint32_t addr;
	char type;
	char name[32];
} mapsym_t;
/* symbol map table */
typedef struct
{
	uint32_t nsyms;
	mapsym_t *sym;
} sysmap_t;
/* load up map */
void init_sysmap( void );
/* return nearest map entry for an address */
int sym_addr( uint32_t addr, mapsym_t *out );
