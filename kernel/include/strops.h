/*
	strops.h : "Basic" string operations.
	(C)2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef STROPS_H
#define STROPS_H
#include <types.h>

/* return lowercase of a character */
char chrlcaps( char c );

/* convert an entire string to lowercase */
char *strlcaps( char *s );

/* return CAPSLOCK'd character */
char chrcaps( char c );

/* MAKE A STRING LOUD */
char *strcaps( char *s );

/* append one string to the end of another */
char *strcat( char *dest, char *src );

/* find first occurence of a character in a string */
char *strchr( char *in, char c );

/* case insensitive version of strchr */
char *strcasechr( char *in, char c );

/* find last occurence of a character in a string */
char *strrchr( char *in, char c );

/* case insensitive version of strrchr */
char *strrcasechr( char *in, char c );

/* compare two strings */
Uint8 strcmp( char *a, char *b );

/* case insensitive version of strcmp */
Uint8 strcasecmp( char *a, char *b );

/* overwrite one string with another */
char *strcpy( char *dest, char *src );

/* return if a string contains any of the characters in a set */
Uint8 strcont( char *in, char *set );

/* case insensitive version of strcont */
Uint8 strcasecont( char *in, char *set );

/* returns the length of a null-terminated string */
Uint32 strlen( char *s );

/* find the first occurence of a string inside another */
char *strstr( char *in, char *s );

/* case insensitive version of strstr */
char *strcasestr( char *in, char *s );

/* find the last occurence of a string inside another */
char *strrstr( char *in, char *s );

/* case insensitive version of strrstr */
char *strrcasestr( char *in, char *s );

/* append a number of characters from one string to the end of another */
char *strncat( char *dest, char *src, Uint32 count );

/* compare a number of characters at the start of a string with another */
Uint8 strncmp( char *a, char *b, Uint32 count );

/* case insensitive version of strncasecmp */
Uint8 strncasecmp( char *a, char *b, Uint32 count );

/* overwrite one string with a number of characters from another */
char *strncpy( char *dest, char *src, Uint32 count );

/* check if a number of characters at the start of a string contains
   any of the characters in a set */
Uint8 strncont( char *in, char *set, Uint32 count );

/* case-insensitive version of strncont */
Uint8 strncasecont( char *in, char *set, Uint32 count );

/* find the first occurence of a string inside a number of characters at the
   beginning of a string */
char *strnstr( char *in, char *s, Uint32 count );

/* case insensitive version of strncasestr */
char *strncasestr( char *in, char *s, Uint32 count );

/* find the last occurence of a string inside a number of characters at the
   beginning of a string */
char *strrnstr( char *in, char *s, Uint32 count );

/* case insensitive version of strrncasestr */
char *strrncasestr( char *in, char *s, Uint32 count );

/* append one string to the end of another while keeping the length of the
   resulting string under a specific limit */
char *strlcat( char *dest, char *src, Uint32 count, Uint32 dmax );

/* append a number of characters from one string to the end of another while
   keeping the length of the resulting string under a specific limit */
char *strnlcat( char *dest, char *src, Uint32 count, Uint32 dmax );

/* parse a string into tokens. for each call it will return the next token
   NOTE: does not work like the standard c function. for subsequent calls you
   have to pass the next token, not NULL */
char *strtok(char *str, char *delim);

#endif
