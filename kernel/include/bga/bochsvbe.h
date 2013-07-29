/*
	bochsvbe.h : Bochs VBE definitions (also available in QEMU).
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
/* memory */
#define BGA_MEM_MB 0x10
#define BGA_MEM_KB 0x4000
#define BGA_MEM    0x1000000
/* idk */
#define BGA_4BPP_SHIFT 22
/* bak stuff */
#define BGA_BANK_ADDR 0xA0000
#define BGA_BANK_SIZE 0x10000
#define BGA_BANK_MAX  250
/* default linear framebuffer address */
#define BGA_LFB_ADDR 0xE0000000
/* limits */
#define BGA_MAX_XRES 2560
#define BGA_MAX_YRES 1600
#define BGA_MAX_BPP  32
/* port number */
#define BGA_PORT 0x01CE
/* registers */
#define BGA_REG_VER     0x00
#define BGA_REG_XRES    0x01
#define BGA_REG_YRES    0x02
#define BGA_REG_BPP     0x03
#define BGA_REG_ENABLE  0x04
#define BGA_REG_BANK    0x05
#define BGA_REG_VXRES   0x06
#define BGA_REG_VYRES   0x07
#define BGA_REG_XOFF    0x08
#define BGA_REG_YOFF    0x09
#define BGA_REG_VMEM64K 0x0A
/* version IDs */
#define BGA_VER0 0xB0C0
#define BGA_VER1 0xB0C1
#define BGA_VER2 0xB0C2
#define BGA_VER3 0xB0C3
#define BGA_VER4 0xB0C4
#define BGA_VER5 0xB0C5
#define BGA_VER6 0xB0C6
#define BGA_VERL BGA_VER0
#define BGA_VERH BGA_VER6
/* bit depths */
#define BGA_BPP_4  0x04
#define BGA_BPP_8  0x08
#define BGA_BPP_15 0x0F
#define BGA_BPP_16 0x10
#define BGA_BPP_24 0x18
#define BGA_BPP_32 0x20
/* flags in enable register */
#define BGA_DISABLED   0x00
#define BGA_ENABLED    0x01
#define BGA_GETCAPS    0x02
#define BGA_8BITDAC    0x20
#define BGA_LFBENABLED 0x40
#define BGA_NOCLEARMEM 0x80
