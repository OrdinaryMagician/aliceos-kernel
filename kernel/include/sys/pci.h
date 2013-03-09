/*
	pci.h : Very basic PCI support.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef PCI_H
#define PCI_H
#include <sys/types.h>
#include <sys/helpers.h>

#define PCI_CONF_ADDR 0xCF8
#define PCI_CONF_DATA 0xCFC

/* PCI config address */
typedef struct
{
	Uint32 zero   : 2; /* always zero */
	Uint32 regno  : 6; /* register number */
	Uint32 funcno : 3; /* function number */
	Uint32 devno  : 5; /* device number */
	Uint32 busno  : 8; /* bus number */
	Uint32        : 7; /* reserved */
	Uint32 enable : 1; /* enable bit */
} pci_addr_t;

/* PCI header type */
typedef struct
{
	Uint8 type : 7; /* device type */
	Uint8 mf   : 1; /* is a multifunction device */
} pci_hdtype_t;

/* PCI built-in self test register */
typedef struct
{
	Uint8 ccode   : 4; /* completion code */
	Uint8         : 2; /* reserved */
	Uint8 start   : 1; /* start test */
	Uint8 capable : 1; /* BIST-capable */
} pci_bist_t;

/* PCI command register */
typedef struct
{
	Uint16 iospace   : 1; /* I/O space */
	Uint16 memspace  : 1; /* memory space */
	Uint16 busmaster : 1; /* bus master */
	Uint16 spcycles  : 1; /* special cycles */
	Uint16 mwinval   : 1; /* memory write and invalidate enable */
	Uint16 vgapsnoop : 1; /* VGA palette snoop */
	Uint16 peresp    : 1; /* parity error response */
	Uint16           : 1; /* reserved */
	Uint16 serr      : 1; /* SERR# enable */
	Uint16 fb2b      : 1; /* fast back-to-back enable */
	Uint16 intr      : 1; /* interrupt disable */
	Uint16           : 5; /* reserved */
} pci_cmd_t;

/* PCI status register */
typedef struct
{
	Uint16         : 3; /* reserved */
	Uint16 intstat : 1; /* interrupt status */
	Uint16 caplist : 1; /* capabilities list */
	Uint16 dblcap  : 1; /* 66MHz capable */
	Uint16         : 1;
	Uint16 fb2bcap : 1; /* fast back-to-back capable */
	Uint16 mdperr  : 1; /* master data parity error */
	Uint16 devselt : 2; /* DEVSEL timing */
	Uint16 stabort : 1; /* signaled target abort */
	Uint16 rtabort : 1; /* received target abort */
	Uint16 rmabort : 1; /* received master abort */
	Uint16 ssyserr : 1; /* signaled system error */
	Uint16 dperror : 1; /* detected parity error */
} pci_stat_t;

/* PCI Base Address Registers (BARs) */
typedef struct
{
	Uint32 zero         : 1;  /* always zero */
	Uint32 type         : 2;  /* type */
	Uint32 prefetchable : 1;  /* prefetchable */
	Uint32 address      : 28; /* 16-byte aligned base address */
} pci_mbar_t;

/* Memory Space BAR types */
#define PCI_MBAR_32BIT 0x00
#define PCI_MBAR_64BIT 0x02
#define PCI_MBAR_RESV  0x01

typedef struct
{
	Uint32 one     : 1;  /* always one */
	Uint32         : 1;  /* reserved */
	Uint32 address : 30; /* 4-byte aligned base address */
} pci_iobar_t;

/* Common section of PCI configuration table */
typedef struct
{
	Uint16 vendorid;     /* vendor ID (0xFFFF if unexistent) */
	Uint16 deviceid;     /* device ID */
	Uint16 command;      /* what command to use (0 is always disconnect) */
	Uint16 status;       /* status information for device */
	Uint8 revisionid;    /* revision ID */
	Uint8 prog_if;       /* specifies a programming interface (if any) */
	Uint8 subclass;      /* what this device does (more specific) */
	Uint8 classcode;     /* what this device does */
	Uint8 cache_linesz;  /* system cache line size in 2-byte words */
	Uint8 latency_timer; /* latency timer in PCI bus clock units */
	Uint8 header_type;   /* layout of the rest of the header */
	Uint8 bist;          /* built-in self test stuff */
} attribute((packed)) pci_dev_t;

/* if header type is 00 (general device) */
typedef struct
{
	Uint32 bar0;         /* base address 0 */
	Uint32 bar1;         /* base address 1 */
	Uint32 bar2;         /* base address 2 */
	Uint32 bar3;         /* base address 3 */
	Uint32 bar4;         /* base address 4 */
	Uint32 bar5;         /* base address 5 */
	Uint32 cbus_cis_ptr; /* pointer to Card Information Structure */
	Uint16 ss_vendorid;  /* subsystem vendor ID */
	Uint16 ss_id;        /* subsystem ID */
	Uint32 erom_addr;    /* expansion ROM base address */
	Uint8 caps_ptr;      /* pointer to capabilities */
	Uint8 resv0;
	Uint8 resv1;
	Uint8 resv2;
	Uint8 resv3;
	Uint8 resv4;
	Uint8 resv5;
	Uint8 resv6;
	Uint8 int_line;      /* IRQ device is connected to (0xFF if none) */
	Uint8 int_pin;       /* Interrupt pin used (0x00 for none) */
	Uint8 min_grant;     /* burst period length (1/4usec units) */
	Uint8 max_latency;   /* access interval to PCI bus (1/4usec units) */
} attribute((packed)) pci_dev00_t;

/* if header type is 01 (PCI to PCI bridge) */
typedef struct
{
	Uint32 bar0;          /* base address 0 */
	Uint32 bar1;          /* base address 1 */
	Uint8 pribusno;       /* primary bus number */
	Uint8 secbusno;       /* secondary bus number */
	Uint8 subbusno;       /* subordinate bus number */
	Uint8 sec_latency;    /* secondary latency timer */
	Uint8 iobase;         /* I/O base  */
	Uint8 iolimit;        /* I/O limit */
	Uint16 sec_status;    /* secondary status */
	Uint16 membase;       /* memory base */
	Uint16 memlimit;      /* memory limit */
	Uint16 pf_membase;    /* prefetchable memory base */
	Uint16 pf_memlimit;   /* prefetchable memory limit */
	Uint32 pf_membase_u;  /* prefetchable memory base upper 32 bits */
	Uint32 pf_memlimit_u; /* prefetchable memory limit upper 32 bits */
	Uint16 iobase_u;      /* I/O base upper 16 bits */
	Uint16 iolimit_u;     /* I/O limit upper 16 bits */
	Uint8 caps_ptr;       /* pointer to capabilities */
	Uint8 resv0;
	Uint8 resv1;
	Uint8 resv2;
	Uint8 int_line;       /* IRQ device is connected to (0xFF if none) */
	Uint8 int_pin;        /* Interrupt pin used (0x00 for none) */
	Uint16 bridgectl;     /* bridge control */
} attribute((packed)) pci_dev01_t;

/* if header type is 02 (PCI to CardBus bridge) */
typedef struct
{
	Uint32 cb_addr;     /* CardBus Socket/ExCa base address */
	Uint8 caps_off;     /* offset to list of capabilities */
	Uint8 resv0;
	Uint16 sec_status;  /* secondary status */
	Uint8 pci_busno;    /* PCI bus number */
	Uint8 cb_busno;     /* CardBus bus number */
	Uint8 sub_busno;    /* Subordinate bus number */
	Uint8 cb_latency;   /* CardBus latency timer */
	Uint32 membase0;    /* memory base address 0 */
	Uint32 memlimit0;   /* memory limit 0 */
	Uint32 membase1;    /* memory base address 1 */
	Uint32 memlimit1;   /* memory limit 1 */
	Uint32 iobase0;     /* I/O base address 0 */
	Uint32 iolimit0;    /* I/O limit 0 */
	Uint32 iobase1;     /* I/O base address 1 */
	Uint32 iolimit1;    /* I/O limit 1 */
	Uint8 int_line;     /* IRQ device is connected to (0xFF if none) */
	Uint8 int_pin;      /* Interrupt pin used (0x00 for none) */
	Uint16 bridgectl;   /* bridge control */
	Uint16 ss_vendorid; /* subsystem vendor ID */
	Uint16 ss_id;       /* subsystem ID */
	Uint32 legacy_base; /* 16-bit PC Card legacy mode base address */
} attribute((packed)) pci_dev02_t;

/* pci configuration (read) */
Uint16 pci_cfg_r( Uint8 bus, Uint8 slot, Uint8 fn, Uint8 off );
/* pci configuration (write) */
void pci_cfg_w( Uint8 bus, Uint8 slot, Uint8 fn, Uint8 off, Uint8 data );

#endif
