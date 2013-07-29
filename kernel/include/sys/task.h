/*
	task.h : Multitasking.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
/* some definitions */
#define USR_CORE   0x0000	/* core user (kernel) */
#define USR_ROOT   0x0100	/* root user (admin) */
#define PID_MIN    0x0001	/* first PID */
#define PID_MAX    0x7FFF	/* highest PID */
#define TSK_ACTIVE 0x01	/* task is active, otherwise frozen */
#define TSK_FINISH 0x02	/* task has finished running and should be erased */
#define TSK_BROKEN 0x04	/* task triggered an exception */
#define TSK_RESV08 0x08	/* reserved flag 4 */
#define TSK_RESV10 0x10	/* reserved flag 5 */
#define TSK_RESV20 0x20	/* reserved flag 6 */
#define TSK_RESV40 0x40	/* reserved flag 7 */
#define TSK_RESV80 0x80	/* reserved flag 8 */
#define TNAME_SZ   256	/* size of task name */
/* savestate struct */
typedef struct sstate
{
	/* register data */
	regs_t regs;
	/* pointer to saved stack */
	uint32_t *stack;
} savestate_t;
/* task struct */
typedef struct task
{
	/* unique process ID */
	uint16_t pid;
	/* task's name */
	char name[TNAME_SZ];
	/* owner user */
	uint16_t owner;
	/* access level of task (0-2) */
	uint8_t level;
	/* task flags */
	uint8_t flags;
	/* page directory for task */
	pdir_t *pdir;
	/* last savestate */
	savestate_t state;
	/* parent task */
	struct task *parent;
	/* next task */
	struct task *next;
} task_t;
/* init tasking subsystem */
void init_tasking( void );
/* the almighty fork */
uint32_t fork( void );
/* get current PID */
uint32_t getpid( void );
/* fork (internal) */
uint32_t sc_fork( regs_t *regs );
/* exit */
uint32_t exit( uint32_t status );
/* exit (internal) */
uint32_t sc_exit( regs_t *regs );
