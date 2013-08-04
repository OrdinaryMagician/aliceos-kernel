/*
	types.h : Type definitions for more comfortable coding.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
/* nul~ */
#define NULL ((void*)0)
/* more comfortable this way */
#ifdef __clang__
typedef unsigned __INT8_TYPE__  uint8_t;
typedef signed   __INT8_TYPE__  int8_t;
typedef unsigned __INT16_TYPE__ uint16_t;
typedef signed   __INT16_TYPE__ int16_t;
typedef unsigned __INT32_TYPE__ uint32_t;
typedef signed   __INT32_TYPE__ int32_t;
typedef unsigned __INT64_TYPE__ uint64_t;
typedef signed   __INT64_TYPE__ int64_t;
#else /* gcc, supposedly */
typedef __UINT8_TYPE__  uint8_t;
typedef __INT8_TYPE__   int8_t;
typedef __UINT16_TYPE__ uint16_t;
typedef __INT16_TYPE__  int16_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __INT32_TYPE__  int32_t;
typedef __UINT64_TYPE__ uint64_t;
typedef __INT64_TYPE__  int64_t;
#endif
/* unicode (wide) char */
typedef uint32_t wchar;
/* limits */
#define SINT8_MAX 127
#define SINT16_MAX 32767
#define SINT32_MAX 2147483647L
#define SINT8_MIN -128
#define SINT16_MIN -32768
#define SINT32_MIN -2147483648L
#define UINT8_MAX 255U
#define UINT16_MAX 65535U
#define UINT32_MAX 4294967295UL
/* maximum number of characters needed for representing types */
/* signs are included for signed types */
#define UINT8LEN 3
#define SINT8LEN 4
#define UINT16LEN 5
#define SINT16LEN 6
#define UINT32LEN 10
#define SINT32LEN 11
