/*
	vgareg.h : VGA register operations.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <types.h>
#include <port.h>

#define VGA_DAC_ADWM 0x3C8 /* DAC Address Write Mode */
#define VGA_DAC_ADRM 0x3C7 /* DAC Address Read Mode */
#define VGA_DAC_DATA 0x3C9 /* DAC Data */
#define VGA_DAC_STAT 0x3C7 /* DAC State */

#define VGA_SEQ 0x3C4 /* Sequencer */

#define VGA_SEQ_RESET 0x00 /* Reset */
#define VGA_SEQ_CMODE 0x01 /* Clocking Mode */
#define VGA_SEQ_MMASK 0x02 /* Map Mask */
#define VGA_SEQ_CMSEL 0x03 /* Character Map Select */
#define VGA_SEQ_SMMOD 0x04 /* Sequencer Memory Mode */

#define VGA_GC 0x3CE /* Graphics Controller */

#define VGA_GC_SETRST 0x00 /* Set/Reset */
#define VGA_GC_ESTRST 0x01 /* Enable Set/Reset */
#define VGA_GC_CLRCMP 0x02 /* Color Compare */
#define VGA_GC_DATROT 0x03 /* Data Rotate */
#define VGA_GC_RDMSEL 0x04 /* Read Map Select */
#define VGA_GC_GFXMOD 0x05 /* Graphics Mode */
#define VGA_GC_MSCGFX 0x06 /* Miscellaneous Graphics */
#define VGA_GC_CDNTCR 0x07 /* Color Don't Care */
#define VGA_GC_BTMASK 0x08 /* Bit Mask */

#define VGA_CRTC 0x3D4 /* CRT Controller */

#define VGA_CRTC_HTOTAL 0x00 /* Horizontal Total */
#define VGA_CRTC_EHDISP 0x01 /* End Horizontal Display */
#define VGA_CRTC_SHBLNK 0x02 /* Start Horizontal Blanking */
#define VGA_CRTC_EHBLNK 0x03 /* End Horizontal Blanking */
#define VGA_CRTC_SHRTRC 0x04 /* Start Horizontal Retrace */
#define VGA_CRTC_EHRTRC 0x05 /* End Horizontal Retrace */
#define VGA_CRTC_VTOTAL 0x06 /* Vertical Total */
#define VGA_CRTC_OVRFLW 0x07 /* Overflow */
#define VGA_CRTC_PTRWSC 0x08 /* Preset Row Scan */
#define VGA_CRTC_MXSCLN 0x09 /* Maximum Scan Line */
#define VGA_CRTC_CSRSTR 0x0A /* Cursor Start */
#define VGA_CRTC_CSREND 0x0B /* Cursor End */
#define VGA_CRTC_STADRH 0x0C /* Start Address High */
#define VGA_CRTC_STADRL 0x0D /* Start Address Low */
#define VGA_CRTC_CSRLCH 0x0E /* Cursor Location High */
#define VGA_CRTC_CSRLCL 0x0F /* Cursor Location Low */
#define VGA_CRTC_VTRTST 0x10 /* Vertical Retrace Start */
#define VGA_CRTC_VTRTEN 0x11 /* Vertical Retrace End */
#define VGA_CRTC_VDSPEN 0x12 /* Vertical Display End */
#define VGA_CRTC_OFFSET 0x13 /* Offset */
#define VGA_CRTC_ULNLOC 0x14 /* Underline Location */
#define VGA_CRTC_SVBLNK 0x15 /* Start Vertical Blanking */
#define VGA_CRTC_EVBLNK 0x16 /* End Vertical Blanking */
#define VGA_CRTC_CRTCMC 0x17 /* CRTC Mode Control */
#define VGA_CRTC_LNCOMP 0x18 /* Line Compare */

/* set the value of a VGA register, return previous value */
Uint8 vga_setreg( Uint16 port, Uint8 index, Uint8 value );

/* get the value of a VGA register */
Uint8 vga_getreg( Uint16 port, Uint8 index );
