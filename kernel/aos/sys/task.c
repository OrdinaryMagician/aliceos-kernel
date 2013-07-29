/*
	task.c : Multitasking.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/types.h>
#include <sys/paging.h>
#include <sys/regs.h>
#include <sys/task.h>
#include <memops.h>
#include <sys/helpers.h>
#include <sys/timer.h>
#include <kmem.h>
#include <strops.h>
#include <berp.h>
#include <sys/syscall.h>
/* paging stuff */
extern pdir_t *kernel_directory;
extern pdir_t *current_directory;
/* task pointers */
task_t *ctask; /* current task */
task_t *task_q; /* start of task list */
/* next available PID */
uint32_t next_pid = PID_MIN;
/* switch over to next task */
static void next_task( void )
{
	task_t *next = ctask->next;
	while ( next )
	{
		if ( next->flags&TSK_ACTIVE )
			break;
		next = next->next;
	}
	ctask = next?next:task_q;
}
/* save state routine */
void save_state( regs_t *regs )
{
	/* save registers */
	memmove(&ctask->state.regs,regs,sizeof(regs_t));
	/* the part where I save the stack comes here */
}
/* load state routine */
void load_state( regs_t *regs )
{
	/* load the registers */
	memmove(regs,&ctask->state.regs,sizeof(regs_t));
	/* the part where I restore the stack comes here */
}
/* the task switcher */
void switch_task( regs_t *regs )
{
	/* dummy, not fully implemented */
	return;
	/* save current state */
	save_state(regs);
	/* switch over to next task */
	next_task();
	/* load new state */
	load_state(regs);
}
/* init tasking subsystem */
void init_tasking( void )
{
	/* add task switcher */
	int_disable();
	timer_add(switch_task,10,0);
	ctask = task_q = kmalloc(sizeof(task_t));
	memset(ctask,0,sizeof(task_t));
	/* some info for PID 1 */
	strcpy(ctask->name,"AliceOS-kcore");
	ctask->pid = next_pid++;
	ctask->owner = USR_CORE;
	ctask->level = 2;
	ctask->flags = TSK_ACTIVE;
	ctask->pdir = current_directory;
	/* allocate space for saved stack */
	ctask->state.stack = kmalloc(8192);
	if ( !ctask->state.stack )
		BERP("Could not allocate stack");
	/* now I'm supposed to relocate the stack */
	int_enable();
}
/* the almighty fork */
uint32_t fork( void )
{
	/* yeah, it doesn't get any simpler than this */
	return sc(SC_FORK,0,0,0);
}
/* get current PID */
uint32_t getpid( void )
{
	return ctask->pid;
}
/* fork (internal) */
uint32_t sc_fork( regs_t *regs )
{
	BERP("Not yet fully implemented");
	/* save current task state */
	save_state(regs);
	/* clone it to a new task */
	task_t *child = kmalloc(sizeof(task_t));
	memmove(child,ctask,sizeof(task_t));
	child->pid = next_pid++;
	child->parent = ctask;
	child->state.regs.eax = 0;
	/* clone stack copy */
	child->state.stack = kmalloc(8192);
	memmove(child->state.stack,ctask->state.stack,8192);
	ctask->next = child;
	return 1;
}
/* exit */
uint32_t exit( uint32_t status )
{
	return sc(SC_EXIT,status,0,0);
}
/* exit (internal) */
uint32_t sc_exit( regs_t *regs )
{
	BERP("Not yet fully implemented");
	if ( getpid() == 1 )
		BERP("Attempt to call exit() from kcore");
	/* set return value for task */
	regs->eax = regs->ebx;
	/* task is finished */
	ctask->flags &= ~TSK_ACTIVE;
	ctask->flags |= TSK_FINISH;
	/* force a task switch */
	switch_task(regs);
	return 0;
}
