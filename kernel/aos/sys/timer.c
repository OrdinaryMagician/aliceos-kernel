/*
	timer.c : programmable timer interface.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/types.h>
#include <sys/regs.h>
#include <sys/timer.h>
#include <sys/irq_handlers.h>
#include <sys/helpers.h>
#include <printk.h>
#include <memops.h>
#include <sys/port.h>
/* internal counter */
static uint32_t ticker = 0;
/* length of each tick in nanoseconds */
static uint32_t tscale = 0;
/* system time frequency */
static uint32_t thz = 0;
/* timer list */
static timer_t timer[TIMERS_SZ];
static uint8_t timers = 0;
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
static void timer_call( timer_t t, regs_t *regs )
{
	if ( !(uint32_t)t.fn ) /* no function set */
		return;
	if ( t.oneshot ) /* one-shot tasks are handled differently */
	{
		if ( !t.interval )
		{
			t.fn(regs);
			timer_rm(t.fn);
			return;
		}
		t.interval--;
		return;
	}
	if ( !t.interval ) /* zero interval */
		return;
	if ( ticker%t.interval ) /* not the right time */
		return;
	/* if all is ok, call the task */
	t.fn(regs);
}
/* function to increment ticker and call all registered tasks */
static void timer_callback( regs_t *regs )
{
	ticker++;
	uint8_t i;
	for ( i=0; i<timers; i++ )
		timer_call(timer[i],regs);
	irq_eoi(PIT_IRQ);
	int_enable();
}
/* initialize the base timer */
void init_timer( uint32_t hz )
{
	printk("Setting base timer to %u hz\n",hz);
	timers = 0;
	memset(&timer[0],0,sizeof(timer_t)*TIMERS_SZ);
	printk(" Registering base IRQ%u handler\n",PIT_IRQ);
	register_irq_handler(PIT_IRQ,&timer_callback);
	uint32_t divisor = 1193180/hz;
	printk(" PIT frequency divisor: %u\n",divisor);
	thz = (divisor*hz*hz)/1193180;
	int32_t error = hz-thz;
	tscale = 1000000000/thz;
	outport_b(0x43,0x36); /* repeating mode */
	uint8_t l = (uint8_t)(divisor&0xFF);
	uint8_t h = (uint8_t)((divisor>>8)&0xFF);
	outport_b(0x40,l);
	outport_b(0x40,h);
	printk(" Base timer set with ~%u hz error\n",abs(error));
}
/* register a timer, return 1 on error, 0 otherwise */
uint8_t timer_add( timerfn_t fn, uint32_t interval, uint8_t oneshot )
{
	if ( timers == TIMERS_SZ ) /* list full */
		return 1;
	timer[timers].fn = fn;
	timer[timers].interval = interval;
	timer[timers].oneshot = oneshot;
	timers++;
	return 0;
}
/* rearrange the timer array, removing a specific timer in the process */
static void timer_rmid( uint8_t id )
{
	memmove(&timer[id],&timer[id+1],
		sizeof(timer_t)*(timers-id));
	timers--;
}
/* unregister a timer, return 1 on error, 0 otherwise */
uint8_t timer_rm( timerfn_t fn )
{
	uint8_t i;
	for ( i=0; i<timers; i++ )
	{
		if ( timer[i].fn != fn )
			continue;
		timer_rmid(i);
		return 0;
	}
	return 1;
}
