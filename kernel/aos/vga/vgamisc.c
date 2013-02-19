/*
	vgamisc.c : miscellaneous VGA functions.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <vga/vgamisc.h>
#include <vga/vgareg.h>
#include <sys/port.h>
#include <sys/helpers.h>

/* plane switch */
void planeswitch( Uint8 p )
{
	Uint8 pmask;
	p &= 3;
	pmask = 1 << p;
	setvgareg(VGA_GC, VGA_GC_RDMSEL,p);
	setvgareg(VGA_SEQ, VGA_SEQ_MMASK,pmask);
}

/* blank screen */
void vgablank( void )
{
	Uint8 reg = getvgareg(VGA_SEQ,VGA_SEQ_CMODE);
	setvgareg(VGA_SEQ,VGA_SEQ_RESET,0x01);
	reg |= 1<<5;
	setvgareg(VGA_SEQ,VGA_SEQ_CMODE,reg);
	setvgareg(VGA_SEQ,VGA_SEQ_RESET,0x03);
}

/* unblank screen */
void vgaunblank( void )
{
	Uint8 reg = getvgareg(VGA_SEQ,VGA_SEQ_CMODE);
	setvgareg(VGA_SEQ,VGA_SEQ_RESET,0x01);
	reg &= ~(1<<5);
	setvgareg(VGA_SEQ,VGA_SEQ_CMODE,reg);
	setvgareg(VGA_SEQ,VGA_SEQ_RESET,0x03);
	reg |= 1<<5;
	setvgareg(VGA_SEQ,VGA_SEQ_CMODE,reg);
}

/* disable text blinking and allow all 16 colors for background */
void disableblink( void )
{
	vgablank();
	inport_b(VGA_INSTAT1);
	outport_b(VGA_AC_I,VGA_AC_ATRM);
	Uint8 reg = inport_b(VGA_AC_R);
	reg &= ~(1<<3);
	inport_b(VGA_INSTAT1);
	outport_b(VGA_AC_I,VGA_AC_ATRM);
	outport_b(VGA_AC_W,reg);
	outport_b(VGA_AC_I,0x20);
	vgaunblank();
}

/* enable text blinking reducing background colors to 8 */
void enableblink( void )
{
	vgablank();
	inport_b(VGA_INSTAT1);
	outport_b(VGA_AC_I,VGA_AC_ATRM);
	Uint8 reg = inport_b(VGA_AC_R);
	reg |= 1<<3;
	inport_b(VGA_INSTAT1);
	outport_b(VGA_AC_I,VGA_AC_ATRM);
	outport_b(VGA_AC_W,reg);
	outport_b(VGA_AC_I,0x20);
	vgaunblank();
}
