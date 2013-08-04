/*
	sh_func.c : Internal shell functions.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/types.h>
#include <sh/sh_func.h>
#include <video/loadfnt.h>
#include <video/loadimg.h>
#include <vga/struct.h>
#include <vga/mode3h.h>
#include <strops.h>
#include <demos.h>
#include <sys/pci.h>
/* prototypes */
static cmd_t sh_help;
static cmd_t sh_listdemos;
static cmd_t sh_rundemo;
static cmd_t sh_clear;
static cmd_t sh_lspci;
/* the supported commands */
cmd_t* sh_cmds[SH_NUMCMDS] =
{
	&sh_help,
	&sh_listdemos,
	&sh_rundemo,
	&sh_clear,
	&sh_lspci,
};
/* functions and their cmd_t structs */
static uint32_t cmd_help( uint32_t argc, char **argv )
{
	uint32_t i;
	if ( argc <= 1 )
	{
		for ( i=0; i<SH_NUMCMDS; i++ )
			mode_3h.fbprintf("%s\n",sh_cmds[i]->name);
		return 0;
	}
	for ( i=0; i<SH_NUMCMDS; i++ )
	{
		if ( !strcmp(sh_cmds[i]->name,argv[1]) )
		{
			mode_3h.fbprintf(sh_cmds[i]->help,sh_cmds[i]->name);
			return 0;
		}
	}
	mode_3h.fbprintf("%s: command not found: %s\n",argv[0],argv[1]);
	return 1;
}
static cmd_t sh_help =
{
	.name = "help",
	.help = "usage: %s [command]\n"
		"without parameters, lists all commands\n"
		"otherwise shows help for a specific command\n",
	.run  = cmd_help,
};
static uint32_t cmd_listdemos( uint32_t argc, char **argv )
{
	argc=0;argv=0;
	listdemos();
	return 0;
}
static cmd_t sh_listdemos =
{
	.name = "listdemos",
	.help = "usage: %s\n"
		"list the available demos\n",
	.run  = cmd_listdemos,
};
static uint32_t cmd_rundemo( uint32_t argc, char **argv )
{
	if ( argc <= 1 )
	{
		mode_3h.fbprintf("usage: %s <demoname>\n",argv[0]);
		return 0;
	}
	uint32_t i;
	for ( i=0; i<DEMO_COUNT; i++ )
	{
		if ( !strcmp(demos[i].name,argv[1]) )
		{
			demos[i].func();
			return 0;
		}
	}
	mode_3h.fbprintf("%s: demo not found: %s\n",argv[0],argv[1]);
	return 1;
}
static cmd_t sh_rundemo =
{
	.name = "rundemo",
	.help = "usage: %s <demoname>\n"
		"runs a specific demo\n",
	.run  = cmd_rundemo,
};
static uint32_t cmd_clear( uint32_t argc, char **argv )
{
	argc=0;argv=0;
	mode_3h.clear();
	mode_3h.fbsetcursor(0,0);
	return 0;
}
static cmd_t sh_clear =
{
	.name = "clear",
	.help = "usage: %s\n"
		"clears whole screen\n",
	.run  = cmd_clear,
};
static void lspci_level2( uint8_t bus, uint8_t slot )
{
	uint16_t i;
	for ( i=slot+1; i<8; i++ )
	{
		if ( !pci_exists(bus,slot,i) )
			continue;
		pci_regs_t regs;
		pci_gethead(bus,slot,i,&regs);
		mode_3h.fbprintf("  %s %s %s (Rev %02x) ",
			pci_vendor(regs.h.vendorid),
			pci_device(regs.h.vendorid,regs.h.deviceid),
			pci_class(regs.h.classcode,regs.h.subclass,
			regs.h.prog_if),regs.h.revisionid);
		mode_3h.fbprintf("[%04x,%04x,%02x,%02x,%02x,%02x]\n",
			regs.h.vendorid,regs.h.deviceid,regs.h.classcode,
			regs.h.subclass,regs.h.prog_if,regs.h.revisionid);
	}
}
static void lspci_level1( uint8_t bus )
{
	uint16_t i;
	for ( i=bus+1; i<32; i++ )
	{
		if ( !pci_exists(bus,i,0) )
			continue;
		pci_regs_t regs;
		pci_gethead(bus,i,0,&regs);
		mode_3h.fbprintf(" %s %s %s (Rev %02x) ",
			pci_vendor(regs.h.vendorid),
			pci_device(regs.h.vendorid,regs.h.deviceid),
			pci_class(regs.h.classcode,regs.h.subclass,
			regs.h.prog_if),regs.h.revisionid);
		mode_3h.fbprintf("[%04x,%04x,%02x,%02x,%02x,%02x]\n",
			regs.h.vendorid,regs.h.deviceid,regs.h.classcode,
			regs.h.subclass,regs.h.prog_if,regs.h.revisionid);
		if ( regs.h.classcode == 0x06 ) /* a bridge, stuff inside */
			lspci_level2(bus,i);
	}
}
static void lspci_level0( void )
{
	uint16_t i;
	for ( i=0; i<256; i++ )
	{
		if ( !pci_exists(i,0,0) )
			continue;
		pci_regs_t regs;
		pci_gethead(i,0,0,&regs);
		mode_3h.fbprintf("%s %s %s (Rev %02x) ",
			pci_vendor(regs.h.vendorid),
			pci_device(regs.h.vendorid,regs.h.deviceid),
			pci_class(regs.h.classcode,regs.h.subclass,
			regs.h.prog_if),regs.h.revisionid);
		mode_3h.fbprintf("[%04x,%04x,%02x,%02x,%02x,%02x]\n",
			regs.h.vendorid,regs.h.deviceid,regs.h.classcode,
			regs.h.subclass,regs.h.prog_if,regs.h.revisionid);
		if ( regs.h.classcode == 0x06 ) /* a bridge, stuff inside */
			lspci_level1(i);
	}
}
static uint32_t cmd_lspci( uint32_t argc, char **argv )
{
	argc=0;argv=0;
	lspci_level0();
	return 0;
}
static cmd_t sh_lspci =
{
	.name = "lspci",
	.help = "usage: %s\n"
		"prints a tree of pci devices\n",
	.run  = cmd_lspci,
};
