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
/* more comfortable this way */
typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef signed short int16_t;
typedef unsigned uint32_t;
typedef signed int32_t;
/* unicode (wide) char */
typedef unsigned wchar;
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
#endif
