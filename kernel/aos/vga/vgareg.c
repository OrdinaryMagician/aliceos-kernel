/*
	vgareg.c : VGA register operations.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <vga/vgareg.h>
#include <sys/port.h>
/* set the value of a VGA register, return previous value */
uint8_t setvgareg( uint16_t port, uint8_t index, uint8_t value )
{
	uint8_t save = inport_b(port);
	outport_b(port,index);
	uint8_t data = inport_b(port+1);
	outport_b(port+1,value);
	outport_b(port,save);
	return data;
}
/* get the value of a VGA register */
uint8_t getvgareg( uint16_t port, uint8_t index )
{
	uint8_t save = inport_b(port);
	outport_b(port,index);
	uint8_t data = inport_b(port+1);
	outport_b(port,save);
	return data;
}
