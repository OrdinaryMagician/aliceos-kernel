/*
	timer.c : programmable timer interface.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/timer.h>
#include <sys/irq_handlers.h>
#include <sys/helpers.h>
#include <sys/port.h>
#include <memops.h>
/* internal counter */
static Uint32 ticker = 0;
/* length of each tick in nanoseconds */
static Uint32 tscale = 0;
/* system time frequency */
static Uint32 thz = 0;
/* task list */
#define TTASKLIST_SZ 16
static ttasklist_t timer_tasklist[TTASKLIST_SZ];
static Uint8 timer_tasks = 0;
/* get current ticks passed */
Uint32 get_ticks( void )
{
	return ticker;
}
/* get current nanoseconds/tick */
Uint32 get_timescale( void )
{
	return tscale;
}
/* get current timer frequency */
Uint32 get_hz( void )
{
	return thz;
}
/* return the equivalent in ticks for some time units */
Uint32 timer_sec( Uint32 s )
{
	return (s*1000000000)/tscale;
}
Uint32 timer_msec( Uint32 m )
{
	return (m*1000000)/tscale;
}
Uint32 timer_usec( Uint32 u )
{
	return (u*1000)/tscale;
}
/* calls a timer task if specific conditions are met */
static void timer_calltask( ttasklist_t tl )
{
	if ( !(Uint32)tl.task ) /* no task function set */
		return;
	if ( tl.oneshot ) /* one-shot tasks are handled differently */
	{
		if ( !tl.interval )
		{
			tl.task();
			timer_rmtask(tl.task);
			return;
		}
		tl.interval--;
		return;
	}
	if ( !tl.interval ) /* zero interval */
		return;
	if ( ticker%tl.interval ) /* not the right time */
		return;
	/* if all is ok, call the task */
	tl.task();
}
/* function to increment ticker and call all registered tasks */
static void timer_callback( regs_t *regs )
{
	ticker++;
	Uint8 i;
	for ( i=0; i<timer_tasks; i++ )
		timer_calltask(timer_tasklist[i]);
	int_enable();
	irq_eoi(0);
}
/* initialize the timer */
void init_timer( Uint32 hz )
{
	timer_tasks = 0;
	memset((Uint8*)&timer_tasklist[0],0,sizeof(ttasklist_t)*TTASKLIST_SZ);
	register_irq_handler(0,&timer_callback);
	Uint32 divisor = 1193180/hz;
	thz = (divisor*hz*hz)/1193180;
	tscale = 1000000000/thz;
	outport_b(0x43,0x36); /* repeating mode */
	Uint8 l = (Uint8)(divisor&0xFF);
	Uint8 h = (Uint8)((divisor>>8)&0xFF);
	outport_b(0x40,l);
	outport_b(0x40,h);
}
/* register a timer task, return 1 on error, 0 otherwise */
Uint8 timer_addtask( ttask_t task, Uint32 interval, Uint8 oneshot )
{
	if ( timer_tasks == TTASKLIST_SZ ) /* tasklist full */
		return 1;
	timer_tasklist[timer_tasks].task = task;
	timer_tasklist[timer_tasks].interval = interval;
	timer_tasklist[timer_tasks].oneshot = oneshot;
	timer_tasks++;
	return 0;
}
/* rearrange the task array, removing a specific task in the process */
static void timer_rmtaskid( Uint8 id )
{
	memmove((Uint8*)&timer_tasklist[id],(Uint8*)&timer_tasklist[id+1],
		sizeof(ttasklist_t)*(timer_tasks-id));
	timer_tasks--;
}
/* unregister a timer task, return 1 on error, 0 otherwise */
Uint8 timer_rmtask( ttask_t task )
{
	Uint8 i;
	for ( i=0; i<timer_tasks; i++ )
	{
		if ( timer_tasklist[i].task != task )
			continue;
		timer_rmtaskid(i);
		return 0;
	}
	return 1;
}
