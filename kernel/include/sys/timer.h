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
	Uint32 interval; /* call interval */
	Uint8 oneshot;   /* disabled after first call */
} ttasklist_t;

/* get current ticks passed */
Uint32 get_ticks( void );
/* get current microseconds/tick */
Uint32 get_timescale( void );
/* get current timer frequency */
Uint32 get_hz( void );
/* return the equivalent in ticks for some time units */
Uint32 timer_sec( Uint32 s );
Uint32 timer_msec( Uint32 m );
Uint32 timer_usec( Uint32 u );
/* initialize the timer */
void init_timer( Uint32 hz );
/* register a timer task, return 1 on error, 0 otherwise */
Uint8 timer_addtask( ttask_t task, Uint32 interval, Uint8 oneshot );
/* unregister a timer task, return 1 on error, 0 otherwise */
Uint8 timer_rmtask( ttask_t task );
#endif
