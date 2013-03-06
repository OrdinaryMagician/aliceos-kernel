/*
	shell.c : AliceOS kernel internal shell.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sh/shell.h>
#include <sh/sh_func.h>
#include <sys/kbd.h>
#include <vga/mode3h.h>
#include <memops.h>
#include <strops.h>
#include <kmem.h>
#include <printk.h>

/* internal variables */
static char *sh_cmdline;
static char *sh_lcmdline;
static Uint32 sh_returned;
static Uint8 sh_enabled;
static Uint8 sh_executing;
static Uint32 sh_gotch;

/* prototypes */
static void sh_init( void );
static void sh_disable( void );
static void sh_enable( void );
static char* sh_lastcmd( void );
static Uint32 sh_lastret( void );

/* shell struct */
shell_t shell = 
{
	.init    = sh_init,
	.disable = sh_disable,
	.enable  = sh_enable,
	.lastcmd = sh_lastcmd,
	.lastret = sh_lastret,
};

static void printPS1( void )
{
	mode_3h.fbprintf("%[01root %[0C~ %[09#%[0F ");
}

/* command parser */
static void sh_parsecmd( void )
{
	Uint32 i;
	Uint32 argc;
	char **argv;

	sh_gotch = 0;
	if ( !sh_cmdline[0] )
	{
		printPS1();
		return;
	}

	sh_executing = 1;

	/* save command */
	strncpy(sh_lcmdline,sh_cmdline,256);

	/* separate arguments */
	char *tk = sh_cmdline;
	argc = 1;
	while ( *(tk = strtok(tk," ")) )
		argc++;
	/* generate parameters for command */
	argv = (char**)kmalloc((argc+1)*sizeof(char*));
	memset((Uint8*)argv,0,(argc+1)*sizeof(char*));
	tk = sh_cmdline;
	i = 0;
	while ( *tk && (i<256) )
	{
		argv[i++] = tk;
		tk = tk+strlen(tk)+1;
	}
	Uint8 found = 0;
	for ( i=0; i<SH_NUMCMDS; i++ )
	{
		if ( !strcmp(sh_cmds[i]->name,argv[0]) )
		{
			found = 1;
			sh_returned = sh_cmds[i]->run(argc,argv);
			break;
		}
	}
	if ( !found )
		mode_3h.fbprintf("shell: command not found: %s\n",argv[0]);

	/* free parameters */
	kfree((Uint32)argv);
	printPS1();
	sh_executing = 0;
	return;
}

/* keyboard hook */
static void sh_getkey( key_t *key )
{
	if ( sh_executing )
		return;
	/* ignore everything */
	if ( !key->pressed )
		return;
	if ( key->code == KB_UP )
	{
		while ( sh_gotch )
		{
			sh_cmdline[--sh_gotch] = 0;
			Sint32 cx, cy;
			Uint16 rx, ry;
			mode_3h.fbgetcursor(&cx,&cy);
			mode_3h.fbgetres(&rx,&ry);
			if ( !cx )
				mode_3h.fbsetcursor(rx-1,cy-1);
			else
				mode_3h.fbsetcursor(cx-1,cy);
			mode_3h.fbputs(" \b");
		}
		mode_3h.fbputs(sh_lcmdline);
		sh_gotch = strlen(sh_lcmdline);
		strncpy(sh_cmdline,sh_lcmdline,256);
		return;
	}
	if ( key->mod&KB_MOD_LCTRL && (key->code == KB_C) )
	{
		sh_gotch = 0;
		sh_cmdline[0] = 0;
		mode_3h.fbputc('\n');
		printPS1();
		return;
	}
	if ( key->chr == '\t' )
		return;
	if ( key->chr == '\b' )
	{
		if ( !sh_gotch )
			return;
		sh_cmdline[--sh_gotch] = 0;
		Sint32 cx, cy;
		Uint16 rx, ry;
		mode_3h.fbgetcursor(&cx,&cy);
		mode_3h.fbgetres(&rx,&ry);
		if ( !cx )
			mode_3h.fbsetcursor(rx-1,cy-1);
		else
			mode_3h.fbsetcursor(cx-1,cy);
		mode_3h.fbputs(" \b");
		return;
	}
	if ( (key->type != 1) || !key->chr )
		return;
	mode_3h.fbputc(key->chr);
	if ( (key->chr == '\n') || (sh_gotch >= 255) )
	{
		sh_cmdline[sh_gotch] = 0;
		sh_parsecmd();
		return;
	}
	sh_cmdline[sh_gotch++] = key->chr;
}

/* Initialize internal shell */
static void sh_init( void )
{
	sh_cmdline = (char*)kmalloc(256);
	memset((Uint8*)sh_cmdline,0,256);
	sh_lcmdline = (char*)kmalloc(256);
	memset((Uint8*)sh_lcmdline,0,256);
	sh_returned = 0;
	sh_enabled = 0;
	sh_executing = 0;
	sh_gotch = 0;
	mode_3h.fbprintf("%[0CAOS Internal Shell initiated\n");
	mode_3h.fbprintf("%[0FType \"help\" for a list of commands\n");
	mode_3h.fbprintf("Type \"help\" <command> for help on using a specific command\n\n");
	printPS1();
}

/* Disable shell (ignore keyboard input) */
static void sh_disable( void )
{
	if ( !sh_enabled )
		return;
	kbd_rmhandler(sh_getkey);
	sh_enabled = 0;
}

/* Enable shell (handle keyboard input) */
static void sh_enable( void )
{
	if ( sh_enabled )
		return;
	kbd_addhandler(sh_getkey);
	sh_enabled = 1;
}

/* Retrieve the last command typed */
static char* sh_lastcmd( void )
{
	char *toret = (char*)kmalloc(256);
	strncpy(toret,sh_lcmdline,256);
	return toret;
}

/* Get the return code of last command */
static Uint32 sh_lastret( void )
{
	return sh_returned;
}
