/*
	helpers.h : Short helper functions and macros.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef HELPERS_H
#define HELPERS_H

#define max(a,b) ((a>b)?a:b)
#define min(a,b) ((a<b)?a:b)
#define clamp(a,b,c) ((a<b)?b:(a>c)?c:a)
#define abs(a) ((a>0)?a:(a*(-1)))

/* What do you mean I have to write all these damn underscores? */
#define asm __asm__
#define volatile __volatile__
#define attribute __attribute__

#endif
