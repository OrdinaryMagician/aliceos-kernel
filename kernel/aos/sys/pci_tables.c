/*
	pci_tables.c : PCI string tables for known devices.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/pci_tables.h>
/* list of known vendors */
pci_vnds_t pci_vnds[] =
{
	{0xFFFF,"Unknown Vendor"},
	{0x8086,"Intel"},
	{0x1234,"Bochs"},
};
/* list of known devices */
pci_devs_t pci_devs[] =
{
	{0xFFFF,0xFFFF,"Unknown Device"},
	{0x1234,0x1111,"Experimental PCI"},
	{0x8086,0x100E,"E1000"},
	{0x8086,0x1237,"82441FX"},
	{0x8086,0x7000,"82371SB"},
	{0x8086,0x7113,"82371AB"},
};
/* list of known classes */
pci_clss_t pci_clss[] =
{
	{0xFF,0xFF,0xFF,"Unknown Class"},
	{0x02,0x00,0x00,"Ethernet Adapter"},
	{0x03,0x00,0x00,"VGA-Compatible Device"},
	{0x06,0x00,0x00,"Host Bridge"},
	{0x06,0x01,0x00,"ISA Bridge"},
	{0x06,0x80,0x00,"Misc Bridge"},
};
