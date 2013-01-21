/*
	cmos.h : basic handling of the system CMOS.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <types.h>
#include <helpers.h>
#include <port.h>

void cmos_dump( Uint8 *here );
