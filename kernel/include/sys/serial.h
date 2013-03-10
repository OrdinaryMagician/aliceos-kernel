/*
	serial.h : Serial port driver.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef SERIAL_H
#define SERIAL_H
#include <sys/types.h>
/* serial ports */
#define SERIAL_A 0x3F8
#define SERIAL_B 0x2F8
#define SERIAL_C 0x3E8
#define SERIAL_D 0x2E8
/* IRQ number for serial */
#define SERIAL_IRQ 4
void serial_on( uint16_t dev );
uint8_t serial_recv_d( uint16_t dev );
char serial_recv( uint16_t dev );
char serial_recv_async( uint16_t dev );
uint8_t serial_transmit_empty( uint16_t dev );
void serial_chr( uint16_t dev, char c );
void serial_str( uint16_t dev, char *s );
void serial_uns( uint16_t dev, uint32_t val, uint16_t width, uint8_t zeropad );
void serial_dec( uint16_t dev, int32_t val, uint16_t width, uint8_t zeropad );
void serial_hex( uint16_t dev, uint32_t val, uint16_t width, uint8_t zeropad );
void serial_oct( uint16_t dev, uint32_t val, uint16_t width, uint8_t zeropad );
#endif
