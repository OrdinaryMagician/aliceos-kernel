/*
	serial.h : Serial port driver.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef SERIAL_H
#define SERIAL_H
#include <types.h>
#include <port.h>

#define SERIAL_A 0x3F8
#define SERIAL_B 0x2F8
#define SERIAL_C 0x3E8
#define SERIAL_D 0x2E8

#define SERIAL_IRQ 4

void serial_on( Uint16 dev );
void serial_ins( void );
Uint8 serial_recv_d( Uint16 dev );
char serial_recv( Uint16 dev );
char serial_recv_async( Uint16 dev );
Uint8 serial_transmit_empty( Uint16 dev );
void serial_chr( Uint16 dev, char c );
void serial_str( Uint16 dev, char *s );
void serial_uns( Uint16 dev, Uint64 val, Uint16 width, Uint8 zeropad );
void serial_dec( Uint16 dev, Sint64 val, Uint16 width, Uint8 zeropad );
void serial_hex( Uint16 dev, Uint64 val, Uint16 width, Uint8 zeropad );
void serial_oct( Uint16 dev, Uint64 val, Uint16 width, Uint8 zeropad );

#endif
