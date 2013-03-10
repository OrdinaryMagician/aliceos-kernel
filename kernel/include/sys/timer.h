/*
	timer.h : programmable timer interface.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef TIMER_H
#define TIMER_H
#include <sys/types.h>
/* timer task */
typedef void (*ttask_t)( void );
typedef struct
{
	ttask_t task;    /* task function */
	uint32_t interval; /* call interval */
	uint8_t oneshot;   /* disabled after first call */
} ttasklist_t;
/* get current ticks passed */
uint32_t get_ticks( void );
/* get current nanoseconds/tick */
uint32_t get_timescale( void );
/* get current timer frequency */
uint32_t get_hz( void );
/* return the equivalent in ticks for some time units */
uint32_t timer_sec( uint32_t s );
uint32_t timer_msec( uint32_t m );
uint32_t timer_usec( uint32_t u );
/* initialize the timer */
void init_timer( uint32_t hz );
/* register a timer task, return 1 on error, 0 otherwise */
uint8_t timer_addtask( ttask_t task, uint32_t interval, uint8_t oneshot );
/* unregister a timer task, return 1 on error, 0 otherwise */
uint8_t timer_rmtask( ttask_t task );
#endif
