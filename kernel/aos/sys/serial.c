/*
	serial.c : Serial port driver.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/serial.h>
#include <sys/port.h>
#include <sys/helpers.h>
void serial_on( Uint16 dev )
{
	outport_b(dev+1,0x00);
	outport_b(dev+3,0x80);
	outport_b(dev,0x03);
	outport_b(dev+1,0x00);
	outport_b(dev+3,0x03);
	outport_b(dev+2,0xC7);
	outport_b(dev+4,0x0B);
}
Uint8 serial_recv_d( Uint16 dev )
{
	return inport_b(dev+5)&1;
}
char serial_recv( Uint16 dev )
{
	while ( serial_recv_d(dev) == 0 );
	return inport_b(dev);
}
char serial_recv_async( Uint16 dev )
{
	return inport_b(dev);
}
Uint8 serial_transmit_empty( Uint16 dev )
{
	return inport_b(dev+5)&0x20;
}
void serial_chr( Uint16 dev, char c )
{
	while ( serial_transmit_empty(dev) == 0 );
	outport_b(dev,c);
}
void serial_str( Uint16 dev, char *s )
{
	while ( *s )
		serial_chr(dev,*(s++));
}
void serial_mchr( Uint16 dev, char c, Sint32 i )
{
	while ( i-- > 0 )
		serial_chr(c,dev);
}
void serial_uns( Uint16 dev, Uint32 val, Uint16 width, Uint8 zeropad )
{
	if ( !width )
	{
		char c[10];
		Sint32 i = 0;
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
		Sint32 i = 0;
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
void serial_dec( Uint16 dev, Sint32 val, Uint16 width, Uint8 zeropad )
{
	Uint8 isneg = (val<0);
	val = abs(val);
	if ( !width )
	{
		char c[10];
		Sint32 i = 0;
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
		Sint32 i = 0;
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
void serial_hex( Uint16 dev, Uint32 val, Uint16 width, Uint8 zeropad )
{
	if ( !width )
	{
		char c[8];
		Sint32 i = 0;
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
		Sint32 i = 0;
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
void serial_oct( Uint16 dev, Uint32 val, Uint16 width, Uint8 zeropad )
{
	if ( !width )
	{
		char c[11];
		Sint32 i = 0;
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
		Sint32 i = 0;
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
