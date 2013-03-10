/*
	pci.c : Very basic PCI support.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/pci.h>
#include <sys/port.h>
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
void pci_cfg_w( uint8_t bus, uint8_t slot, uint8_t fn, uint8_t off, uint8_t data )
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
