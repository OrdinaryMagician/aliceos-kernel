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
#include <printk.h>
#include <memops.h>
/* internal counter */
static uint32_t ticker = 0;
/* length of each tick in nanoseconds */
static uint32_t tscale = 0;
/* system time frequency */
static uint32_t thz = 0;
/* task list */
#define TTASKLIST_SZ 16
static ttasklist_t timer_tasklist[TTASKLIST_SZ];
static uint8_t timer_tasks = 0;
/* get current ticks passed */
uint32_t get_ticks( void )
{
	return ticker;
}
/* get current nanoseconds/tick */
uint32_t get_timescale( void )
{
	return tscale;
}
/* get current timer frequency */
uint32_t get_hz( void )
{
	return thz;
}
/* return the equivalent in ticks for some time units */
uint32_t timer_sec( uint32_t s )
{
	return (s*1000000000)/tscale;
}
uint32_t timer_msec( uint32_t m )
{
	return (m*1000000)/tscale;
}
uint32_t timer_usec( uint32_t u )
{
	return (u*1000)/tscale;
}
/* calls a timer task if specific conditions are met */
static void timer_calltask( ttasklist_t tl )
{
	if ( !(uint32_t)tl.task ) /* no task function set */
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
	uint8_t i;
	for ( i=0; i<timer_tasks; i++ )
		timer_calltask(timer_tasklist[i]);
	int_enable();
	irq_eoi(0);
}
/* initialize the timer */
void init_timer( uint32_t hz )
{
	timer_tasks = 0;
	memset(&timer_tasklist[0],0,sizeof(ttasklist_t)*TTASKLIST_SZ);
	register_irq_handler(0,&timer_callback);
	uint32_t divisor = 1193180/hz;
	thz = (divisor*hz*hz)/1193180;
	tscale = 1000000000/thz;
	outport_b(0x43,0x36); /* repeating mode */
	uint8_t l = (uint8_t)(divisor&0xFF);
	uint8_t h = (uint8_t)((divisor>>8)&0xFF);
	outport_b(0x40,l);
	outport_b(0x40,h);
}
/* register a timer task, return 1 on error, 0 otherwise */
uint8_t timer_addtask( ttask_t task, uint32_t interval, uint8_t oneshot )
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
static void timer_rmtaskid( uint8_t id )
{
	memmove(&timer_tasklist[id],&timer_tasklist[id+1],
		sizeof(ttasklist_t)*(timer_tasks-id));
	timer_tasks--;
}
/* unregister a timer task, return 1 on error, 0 otherwise */
uint8_t timer_rmtask( ttask_t task )
{
	uint8_t i;
	for ( i=0; i<timer_tasks; i++ )
	{
		if ( timer_tasklist[i].task != task )
			continue;
		timer_rmtaskid(i);
		return 0;
	}
	return 1;
}
