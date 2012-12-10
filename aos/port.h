#ifndef PORT_H
#define PORT_H
#include "types.h"
void outb( Uint16 port, Uint8 val );
Uint8 inb( Uint16 port );
Uint16 inw( Uint16 port );
#endif
