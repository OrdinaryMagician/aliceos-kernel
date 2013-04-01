/*
	task.h : Multitasking.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef TASK_H
#define TASK_H
#include <sys/types.h>
#include <sys/paging.h>
/* task struct */
typedef struct task
{
	uint32_t pid;
	uint32_t esp, ebp;
	uint32_t eip;
	pdir_t *pdir;
	struct task *next;
} task_t;
/* init tasking subsystem */
void init_tasking( void );
/* the almighty process creation function */
uint8_t fork( void );
/* move the stack */
void move_stack( void *nstack, uint32_t sz );
/* get current PID */
uint32_t getpid( void );
#endif
