/*
	printk.c : The almighty function.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <printk.h>

/* behind-the-scenes function */
void vaprintk( char *s, va_list args )
{
	Uint8 fg, bg;
	Uint8 col;
	Sint32 x, y;
	Uint8 alt;
	Uint8 neg;
	Uint8 zp;
	Uint16 wide;
	/* the basic loop */
	while ( *s )
	{
		x = 0;
		y = 0;
		vga_getattr(&fg,&bg);
		col = (bg&0x0F);
		vga_curget(&x,&y);
		alt = 0;
		zp = 0;
		wide = 0;
		neg = 0;
		if ( *s != '%' )	/* not an escape, so just print character and pass on to next */
		{
			vga_putc(*(s++));
			continue;
		}
		s++;
		if ( *s == '%' )	/* just a percent sign */
		{
			vga_putc(*(s++));
			continue;
		}
		if ( *s == 's' )	/* string */
		{
			vga_puts((char*)va_arg(args,char*));
			s++;
			continue;
		}
		if ( *s == 'c' )	/* a char */
		{
			vga_putc((char)va_arg(args,int));
			s++;
			continue;
		}
		if ( *s == '#' )	/* alternate form */
		{
			alt = 1;
			s++;
		}
		if ( *s == '0' )	/* zero padding */
		{
			zp = 1;
			s++;
		}
		if ( *s == '[' )	/* AOS exclusive, color change */
		{
			s++;
			if ( ((*s >= '0') && (*s <= '9')) || ((*s >= 'A') && (*s <= 'F')) )
			{
				col = ((col&0x0F)<<4)|(((*s <= '9')?(*s-'0'):((*s+0xA)-'A'))&0x0F);
				s++;
			}
			if ( ((*s >= '0') && (*s <= '9')) || ((*s >= 'A') && (*s <= 'F')) )
			{
				col = ((col&0x0F)<<4)|(((*s <= '9')?(*s-'0'):((*s+0xA)-'A'))&0x0F);
				s++;
			}
			vga_setattr(col&0x0F,(col>>4)&0x0F);
			continue;
		}
		if ( *s == '(' )	/* AOS exclusive, cursor move */
		{
			x = 0;
			y = 0;
			s++;
			if ( *s == '-' )
			{
				neg = 1;
				s++;
			}
			while ( (*s >= '0') && (*s <= '9') )
				x = x*10+(*(s++)-'0');
			if ( neg )
				x *= -1;
			neg = 0;
			if ( *s == ',' )
			{
				s++;
				if ( *s == '-' )
				{
					neg = 1;
					s++;
				}
				while ( (*s >= '0') && (*s <= '9') )
					y = y*10+(*(s++)-'0');
				if ( neg )
					y *= -1;
			}
			vga_curmv(x,y);
			continue;
		}
		if ( *s == '{' )	/* AOS exclusive, cursor set */
		{
			x = 0;
			s++;
			if ( *s == '-' )
			{
				neg = 1;
				s++;
			}
			while ( (*s >= '0') && (*s <= '9') )
				x = x*10+(*(s++)-'0');
			if ( neg )
				x *= -1;
			neg = 0;
			if ( *s == ',' )
			{
				y = 0;
				s++;
				if ( *s == '-' )
				{
					neg = 1;
					s++;
				}
				while ( (*s >= '0') && (*s <= '9') )
					y = y*10+(*(s++)-'0');
				if ( neg )
					y *= -1;
			}
			vga_curset(x,y);
			continue;
		}
		while ( (*s >= '0') && (*s <= '9') )	/* field width */
			wide = wide*10+(*(s++)-'0');
		if ( *s == 'd' )	/* signed base 10 integer */
		{
			vga_putd((signed long)va_arg(args,signed long),wide,zp);
			s++;
			continue;
		}
		if ( *s == 'u' )	/* unsigned base 10 integer */
		{
			vga_putu((unsigned long)va_arg(args,unsigned long),wide,zp);
			s++;
			continue;
		}
		if ( *s == 'x' )	/* unsigned base 16 integer */
		{
			if ( alt )
				vga_puts("0x");
			vga_puth((unsigned long)va_arg(args,unsigned long),wide,zp);
			s++;
			continue;
		}
		if ( *s == 'o' )	/* unsigned base 8 integer */
		{
			if ( alt )
				vga_putc('0');
			vga_puto((unsigned long)va_arg(args,unsigned long),wide,zp);
			s++;
			continue;
		}
		/* whatever */
		vga_putc(*(s++));
	}
}

/* kernel's own personal printf */
void printk( char *s, ... )
{
	va_list args;
	va_start(args,s);
	vaprintk(s,args);
	va_end(args);
}

/* another behind-the-scenes function */
void vaprintk_s( Uint16 dev, char *s, va_list args )
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
		if ( *s != '%' )	/* not an escape, so just print character and pass on to next */
		{
			serial_chr(dev,*(s++));
			continue;
		}
		s++;
		if ( *s == '%' )	/* just a percent sign */
		{
			serial_chr(dev,*(s++));
			continue;
		}
		if ( *s == 's' )	/* string */
		{
			serial_str(dev,(char*)va_arg(args,char*));
			s++;
			continue;
		}
		if ( *s == 'c' )	/* a char */
		{
			serial_chr(dev,(char)va_arg(args,int));
			s++;
			continue;
		}
		if ( *s == '#' )	/* alternate form */
		{
			alt = 1;
			s++;
		}
		if ( *s == '0' )	/* zero padding */
		{
			zp = 1;
			s++;
		}
		while ( (*s >= '0') && (*s <= '9') )	/* field width */
			wide = wide*10+(*(s++)-'0');
		if ( *s == 'd' )	/* signed base 10 integer */
		{
			serial_dec(dev,(signed long)va_arg(args,signed long),wide,zp);
			s++;
			continue;
		}
		if ( *s == 'u' )	/* unsigned base 10 integer */
		{
			serial_uns(dev,(unsigned long)va_arg(args,unsigned long),wide,zp);
			s++;
			continue;
		}
		if ( *s == 'x' )	/* unsigned base 16 integer */
		{
			if ( alt )
				serial_str(dev,"0x");
			serial_hex(dev,(unsigned long)va_arg(args,unsigned long),wide,zp);
			s++;
			continue;
		}
		if ( *s == 'o' )	/* unsigned base 8 integer */
		{
			if ( alt )
				serial_chr(dev,'0');
			serial_oct(dev,(unsigned long)va_arg(args,unsigned long),wide,zp);
			s++;
			continue;
		}
		/* whatever */
		serial_chr(dev,*(s++));
	}
}

/* kernel's own personal printf, for serial */
void printk_s( Uint16 dev, char *s, ... )
{
	va_list args;
	va_start(args,s);
	vaprintk_s(dev,s,args);
	va_end(args);
}
