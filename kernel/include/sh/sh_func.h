/*
	sh_func.h : Internal shell functions.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef SH_FUNC_H
#define SH_FUNC_H
#include <sys/types.h>

/* command struct */
typedef struct
{
	char *name;
	char *help;
	Uint32 (*run)( Uint32 argc, char **argv );
} cmd_t;

#define SH_NUMCMDS 4
cmd_t* sh_cmds[SH_NUMCMDS];
#endif
