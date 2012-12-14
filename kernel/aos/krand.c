/*
	krand.c : Pseudorandom number generator.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <krand.h>

static Uint64 krand_seed = 1;

/* get next random number */
Uint64 krand()
{
	return (krand_seed = krand_seed*195728565+123456789);
}

/* set a specific seed */
void ksrand( Uint64 nseed )
{
	krand_seed = nseed;
}
