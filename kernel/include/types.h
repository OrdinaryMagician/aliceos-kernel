/*
	types.h : Type definitions for more comfortable coding.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef TYPES_H
#define TYPES_H

/* nul~ */
#define NULL ((void*)0)

/* more comfortable this way, I like how SDL does it */
typedef unsigned char  Uint8;
typedef          char  Sint8;
typedef unsigned short Uint16;
typedef          short Sint16;
typedef unsigned int   Uint32;
typedef          int   Sint32;
typedef unsigned long  Uint64;
typedef          long  Sint64;

/* limits */
#define SINT8_MAX 127
#define SINT16_MAX 32767
#define SINT32_MAX 2147483647
#define SINT64_MAX 9223372036854775807L
#define SINT8_MIN -128
#define SINT16_MIN -32768
#define SINT32_MIN -2147483648
#define SINT64_MIN -9223372036854775808L
#define UINT8_MAX 255U
#define UINT16_MAX 65535U
#define UINT32_MAX 4294967295U
#define UINT64_MAX 18446744073709551615UL

/* maximum number of characters needed for representing types */
/* signs are included for signed types */
#define UINT8LEN 3
#define SINT8LEN 4
#define UINT16LEN 5
#define SINT16LEN 6
#define UINT32LEN 10
#define SINT32LEN 11
#define UINT64LEN 20
#define SINT64LEN 21

/* booleans, don't really use it but just in case */

#define _Bool bool;
#define true 1;
#define false 0;

#endif
