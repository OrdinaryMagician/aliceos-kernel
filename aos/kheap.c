#include "kheap.h"

extern Uint32 end;
Uint32 placement_address = (Uint32)&end;

Uint32 kmalloc_int( Uint32 sz, Uint8 align, Uint32 *phys )
{
	if ( align != 0 && (placement_address & 0xFFFFF000) )
	{
		placement_address &= 0xFFFFF000;
		placement_address += 0x1000;
	}
	if ( phys )
		*phys = placement_address;
	Uint32 tmp = placement_address;
	placement_address += sz;
	return tmp;
}

Uint32 kmalloc_a( Uint32 sz )
{
	return kmalloc_int(sz,1,0);
}

Uint32 kmalloc_p( Uint32 sz, Uint32 *phys )
{
	return kmalloc_int(sz,0,phys);
}

Uint32 kmalloc_ap( Uint32 sz, Uint32 *phys )
{
	return kmalloc_int(sz,1,phys);
}

Uint32 kmalloc( Uint32 sz )
{
	return kmalloc_int(sz,0,0);
}
