/*
	krand.c : Pseudorandom number generator.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <krand.h>
static uint32_t krand_seed = 1;
/* get next random number */
uint32_t krand()
{
	return (krand_seed = (krand_seed<<1)*35447+(krand_seed/87));
}
/* set a specific seed */
void ksrand( uint32_t nseed )
{
	krand_seed = nseed;
}
