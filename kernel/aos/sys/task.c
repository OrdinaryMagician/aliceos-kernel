/*
	task.c : Multitasking.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/task.h>
#include <sys/helpers.h>
#include <sys/timer.h>
#include <sys/regs.h>
#include <sys/syscall.h>
#include <strops.h>
#include <berp.h>
#include <printk.h>
#include <kmem.h>
#include <memops.h>
extern uint32_t initial_esp;
extern pdir_t *kernel_directory;
extern pdir_t *current_directory;
/* task pointers */
task_t *ctask; /* current task */
task_t *task_q; /* start of task list */
/* next available PID */
uint32_t next_pid = PID_MIN;
/* flush the TLB */
extern void flush_tlb( void );
/* save state routine */
void save_state( regs_t *regs )
{
}
/* load state routine */
void load_state( regs_t *regs )
{
}
/* the task switcher */
void switch_task( regs_t *regs )
{
}
/* init tasking subsystem */
void init_tasking( void )
{
	/* add task switcher */
	int_disable();
	timer_add(switch_task,10,0);
	ctask = task_q = kmalloc(sizeof(task_t));
	memset(ctask,0,sizeof(task_t));
	strcpy(ctask->name,"AliceOS-kcore");
	ctask->pid = next_pid++;
	ctask->owner = USR_CORE;
	ctask->level = 2;
	ctask->flags = TSK_ACTIVE;
	ctask->pdir = current_directory;
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
	/* save current task state */
	save_state(regs);
	/* clone it to a new task */
	task_t *child = kmalloc(sizeof(task_t));
	memcpy(child,ctask,sizeof(task_t));
	child->pid = next_pid++;
	child->parent = ctask;
	child->state.eax = 0;
	ctask->next = child;
	return 1;
}
/* exit */
uint32_t sc_exit( regs_t *regs )
{
	/* set return value for task */
	regs->eax = regs->ebx;
	/* task is finished */
	ctask->flags |= TSK_FINISH;
	/* force a task switch */
	switch_task(regs);
	return 0;
}
