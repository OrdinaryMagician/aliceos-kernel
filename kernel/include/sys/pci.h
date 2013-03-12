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
/* port I/O */
#define PCI_CONF_ADDR 0xCF8
#define PCI_CONF_DATA 0xCFC
/* PCI config address */
typedef struct
{
	uint32_t zero   : 2; /* always zero */
	uint32_t regno  : 6; /* register number */
	uint32_t funcno : 3; /* function number */
	uint32_t devno  : 5; /* device number */
	uint32_t busno  : 8; /* bus number */
	uint32_t        : 7; /* reserved */
	uint32_t enable : 1; /* enable bit */
} pci_addr_t;
/* PCI header type */
typedef struct
{
	uint8_t type : 7; /* device type */
	uint8_t mf   : 1; /* is a multifunction device */
} pci_hdtype_t;
/* PCI built-in self test register */
typedef struct
{
	uint8_t ccode   : 4; /* completion code */
	uint8_t         : 2; /* reserved */
	uint8_t start   : 1; /* start test */
	uint8_t capable : 1; /* BIST-capable */
} pci_bist_t;
/* PCI command register */
typedef struct
{
	uint16_t iospace   : 1; /* I/O space */
	uint16_t memspace  : 1; /* memory space */
	uint16_t busmaster : 1; /* bus master */
	uint16_t spcycles  : 1; /* special cycles */
	uint16_t mwinval   : 1; /* memory write and invalidate enable */
	uint16_t vgapsnoop : 1; /* VGA palette snoop */
	uint16_t peresp    : 1; /* parity error response */
	uint16_t           : 1; /* reserved */
	uint16_t serr      : 1; /* SERR# enable */
	uint16_t fb2b      : 1; /* fast back-to-back enable */
	uint16_t intr      : 1; /* interrupt disable */
	uint16_t           : 5; /* reserved */
} pci_cmd_t;
/* PCI status register */
typedef struct
{
	uint16_t         : 3; /* reserved */
	uint16_t intstat : 1; /* interrupt status */
	uint16_t caplist : 1; /* capabilities list */
	uint16_t dblcap  : 1; /* 66MHz capable */
	uint16_t         : 1;
	uint16_t fb2bcap : 1; /* fast back-to-back capable */
	uint16_t mdperr  : 1; /* master data parity error */
	uint16_t devselt : 2; /* DEVSEL timing */
	uint16_t stabort : 1; /* signaled target abort */
	uint16_t rtabort : 1; /* received target abort */
	uint16_t rmabort : 1; /* received master abort */
	uint16_t ssyserr : 1; /* signaled system error */
	uint16_t dperror : 1; /* detected parity error */
} pci_stat_t;
/* PCI Base Address Registers (BARs) */
typedef struct
{
	uint32_t zero         : 1;  /* always zero */
	uint32_t type         : 2;  /* type */
	uint32_t prefetchable : 1;  /* prefetchable */
	uint32_t address      : 28; /* 16-byte aligned base address */
} pci_mbar_t;
/* Memory Space BAR types */
#define PCI_MBAR_32BIT 0x00
#define PCI_MBAR_64BIT 0x02
#define PCI_MBAR_RESV  0x01
/* I/O BAR */
typedef struct
{
	uint32_t one     : 1;  /* always one */
	uint32_t         : 1;  /* reserved */
	uint32_t address : 30; /* 4-byte aligned base address */
} pci_iobar_t;
/* Common section of PCI configuration table */
typedef struct
{
	uint16_t vendorid;     /* vendor ID (0xFFFF if unexistent) */
	uint16_t deviceid;     /* device ID */
	uint16_t command;      /* what command to use (0 is disconnect) */
	uint16_t status;       /* status information for device */
	uint8_t revisionid;    /* revision ID */
	uint8_t prog_if;       /* specifies a programming interface (if any) */
	uint8_t subclass;      /* what this device does (more specific) */
	uint8_t classcode;     /* what this device does */
	uint8_t cache_linesz;  /* system cache line size in 2-byte words */
	uint8_t latency_timer; /* latency timer in PCI bus clock units */
	uint8_t header_type;   /* layout of the rest of the header */
	uint8_t bist;          /* built-in self test stuff */
} attribute((packed)) pci_dev_t;
/* if header type is 0 (general device) */
typedef struct
{
	uint32_t bar0;         /* base address 0 */
	uint32_t bar1;         /* base address 1 */
	uint32_t bar2;         /* base address 2 */
	uint32_t bar3;         /* base address 3 */
	uint32_t bar4;         /* base address 4 */
	uint32_t bar5;         /* base address 5 */
	uint32_t cbus_cis_ptr; /* pointer to Card Information Structure */
	uint16_t ss_vendorid;  /* subsystem vendor ID */
	uint16_t ss_id;        /* subsystem ID */
	uint32_t erom_addr;    /* expansion ROM base address */
	uint8_t caps_ptr;      /* pointer to capabilities */
	uint8_t resv0;
	uint8_t resv1;
	uint8_t resv2;
	uint8_t resv3;
	uint8_t resv4;
	uint8_t resv5;
	uint8_t resv6;
	uint8_t int_line;      /* IRQ device is connected to (0xFF if none) */
	uint8_t int_pin;       /* Interrupt pin used (0x00 for none) */
	uint8_t min_grant;     /* burst period length (1/4usec units) */
	uint8_t max_latency;   /* access interval to PCI bus (1/4usec units) */
	uint32_t filler[48];  /* filler so it all fits in 256 bytes */
} attribute((packed)) pci_dev0_t;
/* if header type is 1 (PCI to PCI bridge) */
typedef struct
{
	uint32_t bar0;          /* base address 0 */
	uint32_t bar1;          /* base address 1 */
	uint8_t pribusno;       /* primary bus number */
	uint8_t secbusno;       /* secondary bus number */
	uint8_t subbusno;       /* subordinate bus number */
	uint8_t sec_latency;    /* secondary latency timer */
	uint8_t iobase;         /* I/O base  */
	uint8_t iolimit;        /* I/O limit */
	uint16_t sec_status;    /* secondary status */
	uint16_t membase;       /* memory base */
	uint16_t memlimit;      /* memory limit */
	uint16_t pf_membase;    /* prefetchable memory base */
	uint16_t pf_memlimit;   /* prefetchable memory limit */
	uint32_t pf_membase_u;  /* prefetchable memory base upper 32 bits */
	uint32_t pf_memlimit_u; /* prefetchable memory limit upper 32 bits */
	uint16_t iobase_u;      /* I/O base upper 16 bits */
	uint16_t iolimit_u;     /* I/O limit upper 16 bits */
	uint8_t caps_ptr;       /* pointer to capabilities */
	uint8_t resv0;
	uint8_t resv1;
	uint8_t resv2;
	uint8_t int_line;       /* IRQ device is connected to (0xFF if none) */
	uint8_t int_pin;        /* Interrupt pin used (0x00 for none) */
	uint16_t bridgectl;     /* bridge control */
	uint32_t filler[49];   /* filler so it all fits in 256 bytes */
} attribute((packed)) pci_dev1_t;
/* if header type is 2 (PCI to CardBus bridge) */
typedef struct
{
	uint32_t cb_addr;     /* CardBus Socket/ExCa base address */
	uint8_t caps_off;     /* offset to list of capabilities */
	uint8_t resv0;
	uint16_t sec_status;  /* secondary status */
	uint8_t pci_busno;    /* PCI bus number */
	uint8_t cb_busno;     /* CardBus bus number */
	uint8_t sub_busno;    /* Subordinate bus number */
	uint8_t cb_latency;   /* CardBus latency timer */
	uint32_t membase0;    /* memory base address 0 */
	uint32_t memlimit0;   /* memory limit 0 */
	uint32_t membase1;    /* memory base address 1 */
	uint32_t memlimit1;   /* memory limit 1 */
	uint32_t iobase0;     /* I/O base address 0 */
	uint32_t iolimit0;    /* I/O limit 0 */
	uint32_t iobase1;     /* I/O base address 1 */
	uint32_t iolimit1;    /* I/O limit 1 */
	uint8_t int_line;     /* IRQ device is connected to (0xFF if none) */
	uint8_t int_pin;      /* Interrupt pin used (0x00 for none) */
	uint16_t bridgectl;   /* bridge control */
	uint16_t ss_vendorid; /* subsystem vendor ID */
	uint16_t ss_id;       /* subsystem ID */
	uint32_t legacy_base; /* 16-bit PC Card legacy mode base address */
	uint32_t filler[46];  /* filler so it all fits in 256 bytes */
} attribute((packed)) pci_dev2_t;
/* composite header */
#define PCI_HEADER_SZ 256
typedef struct
{
	pci_dev_t h;
	union
	{
		pci_dev0_t h0;
		pci_dev1_t h1;
		pci_dev2_t h2;
	} s;
} attribute((packed)) pci_regs_t;
/* retrieve certain values from string tables */
char *pci_class( uint8_t cls, uint8_t sub, uint8_t pif );
char *pci_vendor( uint16_t vid );
char *pci_device( uint16_t vid, uint16_t did );
/* pci configuration (read) */
uint16_t pci_cfg_r( uint8_t bus, uint8_t slot, uint8_t fn, uint8_t off );
/* pci configuration (write) */
void pci_cfg_w( uint8_t bus, uint8_t slot, uint8_t fn, uint8_t off,
		uint8_t data );
/* check if a device exists (vendor id not 0xFFFF) */
uint8_t pci_exists( uint8_t bus, uint8_t slot, uint8_t fn );
/* get the header for a specific device, assumes it exists */
void pci_gethead( uint8_t bus, uint8_t slot, uint8_t fn, void *to );
/* enumerate pci devices */
void init_pci( void );
#endif
