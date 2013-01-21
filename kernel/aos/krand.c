/*
	krand.c : Pseudorandom number generator.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <krand.h>

static Uint64 krand_seed = 1;

/* get next random number */
Uint64 krand()
{
	return (krand_seed = (krand_seed<<1)*35447+(krand_seed/83));
}

/* set a specific seed */
void ksrand( Uint64 nseed )
{
	krand_seed = nseed;
}
