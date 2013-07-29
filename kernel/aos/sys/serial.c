/*
	serial.c : Serial port driver.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/types.h>
#include <sys/port.h>
#include <sys/helpers.h>
void serial_on( uint16_t dev )
{
	outport_b(dev+1,0x00);
	outport_b(dev+3,0x80);
	outport_b(dev,0x03);
	outport_b(dev+1,0x00);
	outport_b(dev+3,0x03);
	outport_b(dev+2,0xC7);
	outport_b(dev+4,0x0B);
}
uint8_t serial_recv_d( uint16_t dev )
{
	return inport_b(dev+5)&1;
}
char serial_recv( uint16_t dev )
{
	while ( serial_recv_d(dev) == 0 );
	return inport_b(dev);
}
char serial_recv_async( uint16_t dev )
{
	return inport_b(dev);
}
uint8_t serial_transmit_empty( uint16_t dev )
{
	return inport_b(dev+5)&0x20;
}
void serial_chr( uint16_t dev, char c )
{
	while ( serial_transmit_empty(dev) == 0 );
	outport_b(dev,c);
}
void serial_str( uint16_t dev, char *s )
{
	while ( *s )
		serial_chr(dev,*(s++));
}
void serial_mchr( uint16_t dev, char c, int32_t i )
{
	while ( i-- > 0 )
		serial_chr(c,dev);
}
void serial_uns( uint16_t dev, uint32_t val, uint16_t width, uint8_t zeropad )
{
	if ( !width )
	{
		char c[10];
		int32_t i = 0;
		do
		{
			c[i++] = '0'+val%10;
			val /= 10;
		}
		while ( val != 0 );
		while ( i > 0 )
			serial_chr(dev,c[--i]);
	}
	else
	{
		char c[width];
		int32_t i = 0;
		do
		{
			c[i++] = '0'+val%10;
			val /= 10;
		}
		while ( (val != 0) && (i < width) );
		while ( i < width )
			c[i++] = (zeropad)?'0':' ';
		while ( i > 0 )
			serial_chr(dev,c[--i]);
	}
}
void serial_dec( uint16_t dev, int32_t val, uint16_t width, uint8_t zeropad )
{
	uint8_t isneg = (val<0);
	val = abs(val);
	if ( !width )
	{
		char c[10];
		int32_t i = 0;
		do
		{
			c[i++] = '0'+val%10;
			val /= 10;
		}
		while ( val != 0 );
		if ( isneg )
			serial_chr(dev,'-');
		while ( i > 0 )
			serial_chr(dev,c[--i]);
	}
	else
	{
		char c[width];
		int32_t i = 0;
		do
		{
			c[i++] = '0'+val%10;
			val /= 10;
		}
		while ( (val != 0) && (i < width) );
		if ( isneg )
			serial_chr(dev,'-');
		while ( i < width )
			c[i++] = (zeropad)?'0':' ';
		while ( i > 0 )
			serial_chr(dev,c[--i]);
	}
}
void serial_hex( uint16_t dev, uint32_t val, uint16_t width, uint8_t zeropad )
{
	if ( !width )
	{
		char c[8];
		int32_t i = 0;
		do
		{
			c[i++] = ((val&0x0F)>0x09)?('A'+(val&0x0F)-0x0A)
				:('0'+(val&0x0F));
			val >>= 4;
		}
		while ( val != 0 );
		while ( i > 0 )
			serial_chr(dev,c[--i]);
	}
	else
	{
		char c[width];
		int32_t i = 0;
		do
		{
			c[i++] = ((val&0x0F)>0x09)?('A'+(val&0x0F)-0x0A)
				:('0'+(val&0x0F));
			val >>= 4;
		}
		while ( (val != 0) && (i < width) );
		while ( i < width )
			c[i++] = (zeropad)?'0':' ';
		while ( i > 0 )
			serial_chr(dev,c[--i]);
	}
}
void serial_oct( uint16_t dev, uint32_t val, uint16_t width, uint8_t zeropad )
{
	if ( !width )
	{
		char c[11];
		int32_t i = 0;
		do
		{
			c[i++] ='0'+(val&0x07);
			val >>= 3;
		}
		while ( val != 0 );
		while ( i > 0 )
			serial_chr(dev,c[--i]);
	}
	else
	{
		char c[width];
		int32_t i = 0;
		do
		{
			c[i++] ='0'+(val&0x07);
			val >>= 3;
		}
		while ( (val != 0) && (i < width) );
		while ( i < width )
			c[i++] = (zeropad)?'0':' ';
		while ( i > 0 )
			serial_chr(dev,c[--i]);
	}
}
