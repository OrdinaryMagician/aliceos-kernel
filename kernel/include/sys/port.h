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
Uint8 inport_b( Uint16 port );
void outport_b( Uint16 port, Uint8 data );

/* read/write words */
Uint16 inport_w( Uint16 port );
void outport_w( Uint16 port, Uint16 data );

/* read/write "longs" */
Uint32 inport_l( Uint16 port );
void outport_l( Uint16 port, Uint32 data );

/* read/write multiple words */
void inport_wm( Uint16 port, Uint8 *data, Uint32 size );
void outport_wm( Uint16 port, Uint8 *data, Uint32 size );

#endif
