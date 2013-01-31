/*
	krand.h : Pseudorandom number generator.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef KRAND_H
#define KRAND_H
#include <types.h>

/* get next random number */
Uint32 krand();

/* set a specific seed */
void ksrand( Uint32 nseed );

#endif
