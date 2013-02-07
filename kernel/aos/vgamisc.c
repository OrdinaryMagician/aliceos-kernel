/*
	vgamisc.c : miscellaneous VGA functions.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <vgamisc.h>
#include <port.h>

/* plane switch */
void vga_planeswitch( Uint8 p )
{
	Uint8 pmask;
	p &= 3;
	pmask = 1 << p;
	vga_setreg(VGA_GC, VGA_GC_RDMSEL,p);
	vga_setreg(VGA_SEQ, VGA_SEQ_MMASK,pmask);
}

/* blank screen */
void vga_blank( void )
{
	Uint8 reg = vga_getreg(VGA_SEQ,VGA_SEQ_CMODE);
	vga_setreg(VGA_SEQ,VGA_SEQ_RESET,0x01);
	reg |= 1<<5;
	vga_setreg(VGA_SEQ,VGA_SEQ_CMODE,reg);
	vga_setreg(VGA_SEQ,VGA_SEQ_RESET,0x03);
}

/* unblank screen */
void vga_unblank( void )
{
	Uint8 reg = vga_getreg(VGA_SEQ,VGA_SEQ_CMODE);
	vga_setreg(VGA_SEQ,VGA_SEQ_RESET,0x01);
	reg &= ~(1<<5);
	vga_setreg(VGA_SEQ,VGA_SEQ_CMODE,reg);
	vga_setreg(VGA_SEQ,VGA_SEQ_RESET,0x03);
	reg |= 1<<5;
	vga_setreg(VGA_SEQ,VGA_SEQ_CMODE,reg);
}

/* switch to 8px wide characters */
void vga_set8dot( void )
{
	vga_blank();
	Uint8 reg = vga_getreg(VGA_SEQ,VGA_SEQ_CMODE);
	reg |= 0x01;
	vga_setreg(VGA_SEQ,VGA_SEQ_CMODE,reg);
	reg = inport_b(VGA_MSCOUTR);
	reg &= ~(0x03<<2);
	outport_b(VGA_MSCOUTW,reg);
	vga_unblank();
}

/* switch to 9px wide characters */
void vga_set9dot( void )
{
	vga_blank();
	Uint8 reg = vga_getreg(VGA_SEQ,VGA_SEQ_CMODE);
	reg &= ~(0x01);
	vga_setreg(VGA_SEQ,VGA_SEQ_CMODE,reg);
	reg = inport_b(VGA_MSCOUTR);
	reg &= ~(0x03<<2);
	reg |= 0x01<<2;
	outport_b(VGA_MSCOUTW,reg);
	vga_unblank();
}

/* disable text blinking and allow all 16 colors for background */
void vga_disableblink( void )
{
	vga_blank();
	inport_b(VGA_INSTAT1);
	outport_b(VGA_AC_I,VGA_AC_ATRM);
	Uint8 reg = inport_b(VGA_AC_R);
	reg &= ~(1<<3);
	inport_b(VGA_INSTAT1);
	outport_b(VGA_AC_I,VGA_AC_ATRM);
	outport_b(VGA_AC_W,reg);
	outport_b(VGA_AC_I,0x20);
	vga_unblank();
}

/* enable text blinking reducing background colors to 8 */
void vga_enableblink( void )
{
	vga_blank();
	inport_b(VGA_INSTAT1);
	outport_b(VGA_AC_I,VGA_AC_ATRM);
	Uint8 reg = inport_b(VGA_AC_R);
	reg |= 1<<3;
	inport_b(VGA_INSTAT1);
	outport_b(VGA_AC_I,VGA_AC_ATRM);
	outport_b(VGA_AC_W,reg);
	outport_b(VGA_AC_I,0x20);
	vga_unblank();
}
