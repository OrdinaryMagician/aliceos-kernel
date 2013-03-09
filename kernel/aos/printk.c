/*
	printk.c : The almighty debug function.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <printk.h>
#include <sys/va_list.h>
#include <sys/serial.h>
/* behind-the-scenes function */
static void vaprintk( Uint16 dev, char *s, va_list args )
{
	Uint8 alt;
	Uint8 zp;
	Uint16 wide;
	/* the basic loop */
	while ( *s )
	{
		alt = 0;
		zp = 0;
		wide = 0;
		if ( *s != '%' ) /* not an escape, just print character */
		{
			serial_chr(dev,*(s++));
			continue;
		}
		s++;
		if ( *s == '%' ) /* just a percent sign */
		{
			serial_chr(dev,*(s++));
			continue;
		}
		if ( *s == 's' ) /* string */
		{
			serial_str(dev,(char*)va_arg(args,char*));
			s++;
			continue;
		}
		if ( *s == 'c' ) /* a char */
		{
			serial_chr(dev,(char)va_arg(args,int));
			s++;
			continue;
		}
		if ( *s == '#' ) /* alternate form */
		{
			alt = 1;
			s++;
		}
		if ( *s == '0' ) /* zero padding */
		{
			zp = 1;
			s++;
		}
		while ( (*s >= '0') && (*s <= '9') ) /* field width */
			wide = wide*10+(*(s++)-'0');
		if ( *s == 'd' ) /* signed base 10 integer */
		{
			serial_dec(dev,(Sint32)va_arg(args,Sint32),wide,zp);
			s++;
			continue;
		}
		if ( *s == 'u' ) /* unsigned base 10 integer */
		{
			serial_uns(dev,(Uint32)va_arg(args,Uint32),wide,zp);
			s++;
			continue;
		}
		if ( *s == 'x' ) /* unsigned base 16 integer */
		{
			if ( alt )
				serial_str(dev,"0x");
			serial_hex(dev,(Uint32)va_arg(args,Uint32),wide,zp);
			s++;
			continue;
		}
		if ( *s == 'o' ) /* unsigned base 8 integer */
		{
			if ( alt )
				serial_chr(dev,'0');
			serial_oct(dev,(Uint32)va_arg(args,Uint32),wide,zp);
			s++;
			continue;
		}
		/* whatever */
		serial_chr(dev,*(s++));
	}
}
/* printing to a serial device, for debug purposes */
void printk_s( Uint16 dev, char *s, ... )
{
	va_list args;
	va_start(args,s);
	vaprintk(dev,s,args);
	va_end(args);
}
/* print to the first serial device */
void printk( char *s, ... )
{
	va_list args;
	va_start(args,s);
	vaprintk(SERIAL_A,s,args);
	va_end(args);
}
