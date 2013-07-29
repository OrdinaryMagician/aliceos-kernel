/*
	vgareg.h : VGA register operations.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
/* Attribute Controller ports */
#define VGA_AC_I 0x3C0 /* Address */
#define VGA_AC_W 0x3C0 /* Write */
#define VGA_AC_R 0x3C1 /* Read */
/* Attribute Controller registers */
#define VGA_AC_PAL0 0x00 /* Palette Index 0 */
#define VGA_AC_PAL1 0x01 /* Palette Index 1 */
#define VGA_AC_PAL2 0x02 /* Palette Index 2 */
#define VGA_AC_PAL3 0x03 /* Palette Index 3 */
#define VGA_AC_PAL4 0x04 /* Palette Index 4 */
#define VGA_AC_PAL5 0x05 /* Palette Index 5 */
#define VGA_AC_PAL6 0x06 /* Palette Index 6 */
#define VGA_AC_PAL7 0x07 /* Palette Index 7 */
#define VGA_AC_PAL8 0x08 /* Palette Index 8 */
#define VGA_AC_PAL9 0x09 /* Palette Index 9 */
#define VGA_AC_PALA 0x0A /* Palette Index A */
#define VGA_AC_PALB 0x0B /* Palette Index B */
#define VGA_AC_PALC 0x0C /* Palette Index C */
#define VGA_AC_PALD 0x0D /* Palette Index D */
#define VGA_AC_PALE 0x0E /* Palette Index E */
#define VGA_AC_PALF 0x0F /* Palette Index F */
#define VGA_AC_ATRM 0x10 /* Attribute Mode Control */
#define VGA_AC_OVSC 0x11 /* Overscan Color */
#define VGA_AC_CPLE 0x12 /* Color Plane Enable */
#define VGA_AC_HPXP 0x13 /* Horizontal Pixel Panning */
#define VGA_AC_PXSC 0x14 /* Color Select */
/* DAC ports */
#define VGA_DAC_ADWM 0x3C8 /* Address Write Mode */
#define VGA_DAC_ADRM 0x3C7 /* Address Read Mode */
#define VGA_DAC_DATA 0x3C9 /* Data */
#define VGA_DAC_STAT 0x3C7 /* State */
/* Sequencer address port (add 1 for data port) */
#define VGA_SEQ 0x3C4
/* Sequencer registers */
#define VGA_SEQ_RESET 0x00 /* Reset */
#define VGA_SEQ_CMODE 0x01 /* Clocking Mode */
#define VGA_SEQ_MMASK 0x02 /* Map Mask */
#define VGA_SEQ_CMSEL 0x03 /* Character Map Select */
#define VGA_SEQ_SMMOD 0x04 /* Sequencer Memory Mode */
/* Graphics Controller address port (add 1 for data port */
#define VGA_GC 0x3CE
/* Grapcis Controller registers */
#define VGA_GC_SETRST 0x00 /* Set/Reset */
#define VGA_GC_ESTRST 0x01 /* Enable Set/Reset */
#define VGA_GC_CLRCMP 0x02 /* Color Compare */
#define VGA_GC_DATROT 0x03 /* Data Rotate */
#define VGA_GC_RDMSEL 0x04 /* Read Map Select */
#define VGA_GC_GFXMOD 0x05 /* Graphics Mode */
#define VGA_GC_MSCGFX 0x06 /* Miscellaneous Graphics */
#define VGA_GC_CDNTCR 0x07 /* Color Don't Care */
#define VGA_GC_BTMASK 0x08 /* Bit Mask */
/* CRT Controller address port (add 1 for data port) */
#define VGA_CRTC 0x3D4 
/* CRT Controller registers */
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
/* Miscellaneous Output ports */
#define VGA_MSCOUTR 0x3CC /* Read */
#define VGA_MSCOUTW 0x3C2 /* Write */
/* Feature Control ports */
#define VGA_FTCNTLR 0x3CA /* Read */
#define VGA_FTCNTLW 0x3DA /* Write */
/* Input Status ports */
#define VGA_INSTAT0 0x3C2 /* 0 */
#define VGA_INSTAT1 0x3DA /* 1 */
/* set the value of a VGA register, return previous value */
uint8_t setvgareg( uint16_t port, uint8_t index, uint8_t value );
/* get the value of a VGA register */
uint8_t getvgareg( uint16_t port, uint8_t index );
