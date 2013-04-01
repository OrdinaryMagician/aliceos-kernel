/*
	task.c : Multitasking.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/task.h>
#include <sys/helpers.h>
#include <sys/timer.h>
#include <berp.h>
#include <printk.h>
#include <kmem.h>
#include <memops.h>
/* some inline asm thingies */
#define get_esp(x) asm volatile("mov %%esp,%0":"=r"(x))
#define get_ebp(x) asm volatile("mov %%ebp,%0":"=r"(x))
#define set_esp(x) asm volatile("mov %0,%%esp"::"r"(x))
#define set_ebp(x) asm volatile("mov %0,%%ebp"::"r"(x))
/* when the hell will clang support intel syntax? at&t is ugly as fuck */
extern uint32_t initial_esp;
extern pdir_t *kernel_directory;
extern pdir_t *current_directory;
/* volatile things */
volatile task_t *ctask; /* current task */
volatile task_t *task_q; /* start of task list */
/* next available PID */
uint32_t next_pid = 1;
/* flush the TLB */
extern void flush_tlb( void );
/* move the stack */
void move_stack( void *nstack, uint32_t sz )
{
	uint32_t i;
	for ( i=(uint32_t)nstack; i>=(uint32_t)nstack-sz; i-=0x1000 )
		alloc_frame(get_page(i,1,current_directory),0,1);
	flush_tlb();
	uint32_t ol_esp;
	get_esp(ol_esp);
	uint32_t ol_ebp;
	get_ebp(ol_ebp);
	uint32_t offset = (uint32_t)nstack-initial_esp;
	uint32_t nw_esp = ol_esp+offset;
	uint32_t nw_ebp = ol_ebp+offset;
	/* copy the stack */
	memcpy((void*)nw_esp,(void*)ol_esp,initial_esp-ol_esp);
	/* backtrace and copy values */
	for ( i=(uint32_t)nstack; i>(uint32_t)nstack-sz; i-=4 )
	{
		uint32_t tmp = *(uint32_t*)i;
		if ( (ol_esp<tmp)&&(tmp<initial_esp) )
		{
			/* assuming this is a base pointer, remap it */
			tmp = tmp+offset;
			uint32_t *tmp2 = (uint32_t*)i;
			*tmp2 = tmp;
		}
	}
	/* change the pointers */
	set_esp(nw_esp);
	set_ebp(nw_ebp);
}
/* read EIP */
extern uint32_t read_eip( void );
/* task switching */
static void switch_task( void )
{
	/* no tasking yet */
	if ( !ctask )
		return;
	int_disable();
	uint32_t esp, ebp, eip;
	get_esp(esp);
	get_ebp(ebp);
	eip = read_eip();
	/* we actually switched tasks */
	if ( eip == 0x12345 )
		return;
	/* nope, let's do it */
	ctask->eip = eip;
	ctask->esp = esp;
	ctask->ebp = ebp;
	ctask = ctask->next;
	if ( !ctask ) /* at the end */
		ctask = task_q;
	esp = ctask->esp;
	ebp = ctask->ebp;
	/* disgusting inline asm */
	asm volatile("	mov %0,%%ecx;\
			mov %1,%%esp;\
			mov %2,%%ebp;\
			mov %3,%%cr3;\
			mov $0x12345,%%eax;\
			sti;\
			jmp *%%ecx"
			::"r"(eip),"r"(esp),"r"(ebp),
			"r"(current_directory->physAddr));
}
/* init tasking subsystem */
void init_tasking( void )
{
	/* add task switcher */
	timer_addtask(switch_task,10,0);
	int_disable();
	move_stack((void*)0xE0000000,0x2000);
	ctask = task_q = kmalloc(sizeof(task_t));
	ctask->pid = next_pid++;
	ctask->esp = ctask->ebp = ctask->eip = 0;
	ctask->pdir = current_directory;
	ctask->next = 0;
	int_enable();
}
/* the almighty process creation function */
uint8_t fork( void )
{
	int_disable();
	task_t *ptask = (task_t*)ctask;
	pdir_t *dir = clone_directory(current_directory);
	/* create new task */
	task_t *ntask = kmalloc(sizeof(task_t));
	ntask->pid = next_pid++;
	ntask->esp = ntask->ebp = ntask->eip = 0;
	ntask->pdir = dir;
	ntask->next = 0;
	/* find tasklist end */
	task_t *ttask = (task_t*)task_q;
	while ( ttask->next )
		ttask = ttask->next;
	/* append */
	ttask->next = ntask;
	uint32_t eip = read_eip();
	if ( ctask == ptask )
	{
		/* still the parent */
		get_esp(ntask->esp);
		get_ebp(ntask->ebp);
		ntask->eip = eip;
		int_enable();
		return ntask->pid;
	}
	/* we forked */
	return 0;
}
/* get current PID */
uint32_t getpid( void )
{
	return ctask->pid;
}
