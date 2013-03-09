/*
	pci.c : Very basic PCI support.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/pci.h>
#include <sys/port.h>

/* pci configuration (read) */
Uint16 pci_cfg_r( Uint8 bus, Uint8 slot, Uint8 fn, Uint8 off )
{
	pci_addr_t addr;
	addr.zero = 0;
	addr.regno = off>>2;
	addr.funcno = fn;
	addr.devno = slot;
	addr.busno = bus;
	addr.enable = 1;
	outport_l(PCI_CONF_ADDR,*(Uint32*)&addr);
	return (inport_l(PCI_CONF_DATA)>>((off&2)*8))&0xFFFF;
}

/* pci configuration (write) */
void pci_cfg_w( Uint8 bus, Uint8 slot, Uint8 fn, Uint8 off, Uint8 data )
{
	pci_addr_t addr;
	addr.zero = 0;
	addr.regno = off>>2;
	addr.funcno = fn;
	addr.devno = slot;
	addr.busno = bus;
	addr.enable = 1;
	outport_l(PCI_CONF_ADDR,*(Uint32*)&addr);
	outport_l(PCI_CONF_DATA,data);
}
