/*
	memops.h : Basic memory block operations.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef MEMOPS_H
#define MEMOPS_H
#include <sys/types.h>
/* copy byte blocks */
uint8_t *memcpy( uint8_t *dest, const uint8_t *src, uint32_t count );
/* copy byte blocks checking for possible overlap */
uint8_t *memmove( uint8_t *dest, const uint8_t *src, uint32_t count );
/* copy word blocks */
uint16_t *memcpyw( uint16_t *dest, const uint16_t *src, uint32_t count );
/* copy word blocks checking for possible overlap */
uint16_t *memmovew( uint16_t *dest, const uint16_t *src, uint32_t count );
/* fill an entire memory block with a repeating byte */
uint8_t *memset( uint8_t *dest, uint8_t val, uint32_t count );
/* fill an entire memory block with a repeating word */
uint16_t *memsetw( uint16_t *dest, uint16_t val, uint32_t count );
/* compare memory blocks, returns 0 if equal, 1 if different */
uint8_t memcmp( uint8_t *a, uint8_t *b, uint32_t count );
/* compare memory blocks by words, returns 0 if equal, 1 if different */
uint8_t memcmpw( uint16_t *a, uint16_t *b, uint32_t count );
/* reverse a memory block */
uint8_t *memrev( uint8_t *dest, uint32_t count );
/* reverse a memory block by words */
uint16_t *memrevw( uint16_t *dest, uint32_t count );
/* find first occurence of a specific byte in a memory block */
/* return 0 if not found, or the offset plus one if found */
uint32_t memb( uint8_t *in, uint8_t b, uint32_t count );
/* find first occurence of a specific word in a memory block */
/* return 0 if not found, or the offset plus one if found */
uint32_t memw( uint16_t *in, uint16_t w, uint32_t count );
#endif
