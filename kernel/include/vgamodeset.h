/*
	vgamodeset.h : Full-on mode setting.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef VGAMODESET_H
#define VGAMODESET_H
#include <types.h>
#include <vgatext.h>
#include <vgareg.h>
#include <vgafont.h>
#include <memops.h>
#include <vgamisc.h>

#define MODE_UNKN 0
#define MODE_3H   1
#define MODE_12H  2
#define MODE_13H  3

/* set video mode - return 1 on failure, 0 on success */
Uint8 vga_modeset( Uint8 mode );
#endif
