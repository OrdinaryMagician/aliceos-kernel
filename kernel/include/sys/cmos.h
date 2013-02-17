/*
	cmos.h : basic handling of the system CMOS.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef CMOS_H
#define CMOS_H
#include <sys/types.h>

void cmos_dump( Uint8 *here );
#endif
