/*
	krand.h : Pseudorandom number generator.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
/* get next random number */
uint32_t krand();
/* set a specific seed */
void ksrand( uint32_t nseed );
