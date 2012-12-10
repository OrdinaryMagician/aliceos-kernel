#ifndef PORT_H
#define PORT_H
void outb( unsigned short port, unsigned char val );
unsigned char inb( unsigned short port );
unsigned short inw( unsigned short port );
#endif
