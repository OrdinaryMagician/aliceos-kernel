/*
	types.h : Type definitions for more comfortable coding.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef TYPES_H
#define TYPES_H
/* nul~ */
#define NULL ((void*)0)
/* more comfortable this way, I like how SDL does it */
typedef unsigned char Uint8;
typedef signed char Sint8;
typedef unsigned short int Uint16;
typedef signed short int Sint16;
typedef unsigned int Uint32;
typedef signed int Sint32;
/* unicode (wide) char */
typedef unsigned int wchar;
/* limits */
#define SINT8_MAX 127
#define SINT16_MAX 32767
#define SINT32_MAX 2147483647
#define SINT8_MIN -128
#define SINT16_MIN -32768
#define SINT32_MIN -2147483648
#define UINT8_MAX 255U
#define UINT16_MAX 65535U
#define UINT32_MAX 4294967295U
/* maximum number of characters needed for representing types */
/* signs are included for signed types */
#define UINT8LEN 3
#define SINT8LEN 4
#define UINT16LEN 5
#define SINT16LEN 6
#define UINT32LEN 10
#define SINT32LEN 11
/* booleans, don't really use it but just in case */
#define _Bool bool
#define true 1
#define false 0
#endif
