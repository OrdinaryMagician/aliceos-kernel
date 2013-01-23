/*
	vgareg.c : VGA register operations.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <vgareg.h>

/* set the value of a VGA register, return previous value */
Uint8 vga_setreg( Uint16 port, Uint8 index, Uint8 value )
{
	Uint8 save = inport_b(port);
	outport_b(port,index);
	Uint8 data = inport_b(port+1);
	outport_b(port+1,value);
	outport_b(port,save);
	return data;
}

/* get the value of a VGA register */
Uint8 vga_getreg( Uint16 port, Uint8 index )
{
	Uint8 save = inport_b(port);
	outport_b(port,index);
	Uint8 data = inport_b(port+1);
	outport_b(port,save);
	return data;
}
