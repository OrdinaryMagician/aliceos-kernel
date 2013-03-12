/*
	pci.c : Very basic PCI support.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/pci.h>
#include <sys/pci_tables.h>
#include <sys/port.h>
#include <printk.h>
/* retrieve certain values from string tables */
extern pci_vnds_t pci_vnds[];
extern pci_devs_t pci_devs[];
extern pci_clss_t pci_clss[];
char *pci_class( uint8_t cls, uint8_t sub, uint8_t pif )
{
	uint32_t i;
	for ( i=0; pci_clss[i].str; i++ )
	{
		if ( pci_clss[i].cls != cls )
			continue;
		if (pci_clss[i].sub != sub )
			continue;
		if ( pci_clss[i].pif == 0xFF )
			return pci_clss[i].str;
		if ( pci_clss[i].pif != pif )
			continue;
		return pci_clss[i].str;
	}
	return pci_clss[0].str;
}
char *pci_vendor( uint16_t vid )
{
	uint32_t i;
	for ( i=0; pci_vnds[i].str; i++ )
	{
		if ( pci_vnds[i].vid != vid )
			continue;
		return pci_vnds[i].str;
	}
	return pci_vnds[0].str;
}
char *pci_device( uint16_t vid, uint16_t did )
{
	uint32_t i;
	for ( i=0; pci_devs[i].str; i++ )
	{
		if ( pci_devs[i].vid != vid )
			continue;
		if ( pci_devs[i].did != did )
			continue;
		return pci_devs[i].str;
	}
	return pci_devs[0].str;
}
/* pci configuration (read) */
uint16_t pci_cfg_r( uint8_t bus, uint8_t slot, uint8_t fn, uint8_t off )
{
	pci_addr_t addr;
	addr.zero = 0;
	addr.regno = off>>2;
	addr.funcno = fn;
	addr.devno = slot;
	addr.busno = bus;
	addr.enable = 1;
	outport_l(PCI_CONF_ADDR,*(uint32_t*)&addr);
	return (inport_l(PCI_CONF_DATA)>>((off&2)*8))&0xFFFF;
}
/* pci configuration (write) */
void pci_cfg_w( uint8_t bus, uint8_t slot, uint8_t fn, uint8_t off,
		uint8_t data )
{
	pci_addr_t addr;
	addr.zero = 0;
	addr.regno = off>>2;
	addr.funcno = fn;
	addr.devno = slot;
	addr.busno = bus;
	addr.enable = 1;
	outport_l(PCI_CONF_ADDR,*(uint32_t*)&addr);
	outport_l(PCI_CONF_DATA,data);
}
/* check if a device exists (vendor id not 0xFFFF) */
uint8_t pci_exists( uint8_t bus, uint8_t slot, uint8_t fn )
{
	if ( pci_cfg_r(bus,slot,fn,0) == 0xFFFF )
		return 0;
	return 1;
}
/* get the header for a specific device, assumes it exists */
void pci_gethead( uint8_t bus, uint8_t slot, uint8_t fn, void *to )
{
	if ( !to )
		return;
	uint16_t *regs = (uint16_t*)to;
	uint16_t i = 0;
	uint16_t sz = PCI_HEADER_SZ/2;
	while ( i<sz )
		*(regs++) = pci_cfg_r(bus,slot,fn,(i++)*2);
}
/* scan functions in a slot */
static void pci_level2scan( uint8_t bus, uint8_t slot )
{
	uint16_t i;
	for ( i=slot+1; i<8; i++ )
	{
		if ( !pci_exists(bus,slot,i) )
			continue;
		pci_regs_t regs;
		pci_gethead(bus,slot,i,&regs);
		printk("  %s %s %s (Rev %02x)\n",pci_vendor(regs.h.vendorid),
			pci_device(regs.h.vendorid,regs.h.deviceid),
			pci_class(regs.h.classcode,regs.h.subclass,
			regs.h.prog_if),regs.h.revisionid);
	}
}
/* scan slots in a bus */
static void pci_level1scan( uint8_t bus )
{
	uint16_t i;
	for ( i=bus+1; i<32; i++ )
	{
		if ( !pci_exists(bus,i,0) )
			continue;
		pci_regs_t regs;
		pci_gethead(bus,i,0,&regs);
		printk(" %s %s %s (Rev %02x)\n",pci_vendor(regs.h.vendorid),
			pci_device(regs.h.vendorid,regs.h.deviceid),
			pci_class(regs.h.classcode,regs.h.subclass,
			regs.h.prog_if),regs.h.revisionid);
		if ( regs.h.header_type&0x80 ) /* multifunction, more stuff */
			pci_level2scan(bus,i);
	}
}
/* scan buses */
static void pci_level0scan( void )
{
	uint16_t i;
	for ( i=0; i<256; i++ )
	{
		if ( !pci_exists(i,0,0) )
			continue;
		pci_regs_t regs;
		pci_gethead(i,0,0,&regs);
		printk("%s %s %s (Rev %02x)\n",pci_vendor(regs.h.vendorid),
			pci_device(regs.h.vendorid,regs.h.deviceid),
			pci_class(regs.h.classcode,regs.h.subclass,
			regs.h.prog_if),regs.h.revisionid);
		if ( regs.h.classcode == 0x06 ) /* a bridge, stuff inside */
			pci_level1scan(i);
	}
}
/* enumerate pci devices */
void init_pci( void )
{
	/* begin scanning for devices */
	pci_level0scan();
}
