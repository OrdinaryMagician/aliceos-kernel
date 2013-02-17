/*
	pic.c : programmable interrupt controller.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/pic.h>
#include <sys/port.h>

/* send end-of-interrupt */
void pic_eoi( Uint8 irq )
{
	if ( irq >= 8 )
		outport_b(PIC2,PIC_EOI);
	outport_b(PIC1,PIC_EOI);
}

/* reinitialize the PIC controllers */
void pic_remap( Uint8 off1, Uint8 off2 )
{
	/* this code will look confusing, sorry for the inconvenience */
	outport_b(PIC1,0x11);
	outport_b(PIC2,0x11);
	outport_b(PIC1+1,off1);
	outport_b(PIC2+1,off2);
	outport_b(PIC1+1,0x04);
	outport_b(PIC2+1,0x02);
	outport_b(PIC1+1,0x01);
	outport_b(PIC2+1,0x01);
	outport_b(PIC1+1,0x00);
	outport_b(PIC2+1,0x00);
}

/* disable the PIC */
void pic_disable( void )
{
	outport_b(PIC2+1,0xFF);
	outport_b(PIC1+1,0xFF);
}
