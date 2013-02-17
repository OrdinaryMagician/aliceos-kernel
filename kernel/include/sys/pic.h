/*
	pic.h : programmable interrupt controller.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef PIC_H
#define PIC_H
#include <sys/types.h>

#define PIC1 0x20
#define PIC2 0xA0

#define PIC_EOI 0x20

/* send end-of-interrupt */
void pic_eoi( Uint8 irq );
/* reinitialize the PIC controllers */
void pic_remap( Uint8 off1, Uint8 off2 );
/* disable the PIC */
void pic_disable( void );

#endif
