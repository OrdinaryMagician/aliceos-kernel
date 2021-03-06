/*
	shell.h : AliceOS kernel internal shell.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
/* basic shell struct */
typedef struct
{
	void (*init)( void );      /* Initialize internal shell */
	void (*disable)( void );   /* Disable shell (ignore keyboard input) */
	void (*enable)( void );    /* Enable shell (handle keyboard input) */
	char* (*lastcmd)( void );  /* Retrieve the last command typed */
	uint32_t (*lastret)( void ); /* Get the return code of last command */
} shell_t;
shell_t shell; /* the AOS internal shell */
