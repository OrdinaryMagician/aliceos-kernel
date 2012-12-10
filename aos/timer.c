/*
	timer.c : Programmable Interval Timer.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include "timer.h"
#include "isr.h"
#include "screen.h"
#include "port.h"

Uint64 tick = 0;

static void timer_callback( registers_t regs )
{
	tick++;
	Uint8 ofg, obg;
	int ox, oy, i;
	text_getcolor(&ofg,&obg);
	cur_get(&ox,&oy);
	cur_set(-21,24);
	text_setcolor(DGRAY,BLACK);
	clear_line_r();
	putlu(tick);
	text_setcolor(ofg,obg);
	cur_set(ox,oy);
}

void init_timer( Uint32 frequency )
{
	register_interrupt_handler(IRQ0,&timer_callback);
	Uint32 divisor = 1193180/frequency;
	outb(0x43,0x36);
	Uint8 l = (Uint8)(divisor&0xFF);
	Uint8 h = (Uint8)((divisor>>8)&0xFF);
	outb(0x40,l);
	outb(0x40,h);
}
