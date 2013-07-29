/*
	sh_func.h : Internal shell functions.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
/* command struct */
typedef struct
{
	char *name;
	char *help;
	uint32_t (*run)( uint32_t argc, char **argv );
} cmd_t;
#define SH_NUMCMDS 5
cmd_t* sh_cmds[SH_NUMCMDS];
