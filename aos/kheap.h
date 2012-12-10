#ifndef KHEAP_H
#define KHEAP_H
#include "types.h"
Uint32 kmalloc_a( Uint32 sz );
Uint32 kmalloc_p( Uint32 sz, Uint32 *phys );
Uint32 kmalloc_ap( Uint32 sz, Uint32 *phys );
Uint32 kmalloc( Uint32 sz );
#endif
