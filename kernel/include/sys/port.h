/*
	port.h : I/O port functions.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef PORT_H
#define PORT_H
#include <sys/types.h>
/* read/write bytes */
extern uint8_t inport_b( uint16_t port );
extern void outport_b( uint16_t port, uint8_t data );
/* read/write words */
extern uint16_t inport_w( uint16_t port );
extern void outport_w( uint16_t port, uint16_t data );
/* read/write "longs" */
extern uint32_t inport_l( uint16_t port );
extern void outport_l( uint16_t port, uint32_t data );
#endif
