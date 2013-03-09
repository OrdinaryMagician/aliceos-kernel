/*
	krand.c : Pseudorandom number generator.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <krand.h>
static Uint32 krand_seed = 1;
/* get next random number */
Uint32 krand()
{
	return (krand_seed = (krand_seed<<1)*35447+(krand_seed/87));
}
/* set a specific seed */
void ksrand( Uint32 nseed )
{
	krand_seed = nseed;
}
