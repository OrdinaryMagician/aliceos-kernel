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
extern Uint8 inport_b( Uint16 port );
extern void outport_b( Uint16 port, Uint8 data );

/* read/write words */
extern Uint16 inport_w( Uint16 port );
extern void outport_w( Uint16 port, Uint16 data );

/* read/write "longs" */
extern Uint32 inport_l( Uint16 port );
extern void outport_l( Uint16 port, Uint32 data );

#endif
