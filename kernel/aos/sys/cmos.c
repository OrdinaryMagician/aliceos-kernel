/*
	cmos.c : basic handling of the system CMOS.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/types.h>
#include <sys/port.h>
void cmos_dump( uint8_t *here )
{
	uint8_t i;
	for ( i=0; i<128; i++ )
	{
		outport_b(0x70,i);
		here[i] = inport_b(0x71);
	}
}
