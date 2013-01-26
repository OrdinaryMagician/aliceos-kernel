/*
	berp.h : Unrecoverable error handling.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef BERP_H
#define BERP_H
#include <types.h>
#include <helpers.h>
#include <printk.h>
#include <vgatext.h>
#include <vgamodeset.h>
#include <vgafont.h>
#include <palettes.h>
#define BERP(msg) berp(msg,__FILE__,__LINE__);
extern void berp( const char *message, const char *file, Uint32 line );
#endif
