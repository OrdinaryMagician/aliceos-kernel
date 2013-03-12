/*
	pci_tables.h : PCI string tables for known devices.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef PCI_TABLES_H
#define PCI_TABLES_H
#include <sys/types.h>
/* vendors */
typedef struct
{
	uint16_t vid;
	char *str;
} pci_vnds_t;
/* devices */
typedef struct
{
	uint16_t vid;
	uint16_t did;
	char *str;
} pci_devs_t;
/* device classes */
typedef struct
{
	uint8_t cls;
	uint8_t sub;
	uint8_t pif; /* 0xFF is any */
	char *str;
} pci_clss_t;
#endif
