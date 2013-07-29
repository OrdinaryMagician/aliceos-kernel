/*
	strops.h : "Basic" string operations.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
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
uint8_t strcmp( char *a, char *b );
/* case insensitive version of strcmp */
uint8_t strcasecmp( char *a, char *b );
/* overwrite one string with another */
char *strcpy( char *dest, char *src );
/* return if a string contains any of the characters in a set */
uint8_t strcont( char *in, char *set );
/* case insensitive version of strcont */
uint8_t strcasecont( char *in, char *set );
/* returns the length of a null-terminated string */
uint32_t strlen( char *s );
/* find the first occurence of a string inside another */
char *strstr( char *in, char *s );
/* case insensitive version of strstr */
char *strcasestr( char *in, char *s );
/* find the last occurence of a string inside another */
char *strrstr( char *in, char *s );
/* case insensitive version of strrstr */
char *strrcasestr( char *in, char *s );
/* append a number of characters from one string to the end of another */
char *strncat( char *dest, char *src, uint32_t count );
/* compare a number of characters at the start of a string with another */
uint8_t strncmp( char *a, char *b, uint32_t count );
/* case insensitive version of strncasecmp */
uint8_t strncasecmp( char *a, char *b, uint32_t count );
/* overwrite one string with a number of characters from another */
char *strncpy( char *dest, char *src, uint32_t count );
/* check if a number of characters at the start of a string contains
   any of the characters in a set */
uint8_t strncont( char *in, char *set, uint32_t count );
/* case-insensitive version of strncont */
uint8_t strncasecont( char *in, char *set, uint32_t count );
/* find the first occurence of a string inside a number of characters at the
   beginning of a string */
char *strnstr( char *in, char *s, uint32_t count );
/* case insensitive version of strncasestr */
char *strncasestr( char *in, char *s, uint32_t count );
/* find the last occurence of a string inside a number of characters at the
   beginning of a string */
char *strrnstr( char *in, char *s, uint32_t count );
/* case insensitive version of strrncasestr */
char *strrncasestr( char *in, char *s, uint32_t count );
/* append one string to the end of another while keeping the length of the
   resulting string under a specific limit */
char *strlcat( char *dest, char *src, uint32_t dmax );
/* append a number of characters from one string to the end of another while
   keeping the length of the resulting string under a specific limit */
char *strnlcat( char *dest, char *src, uint32_t count, uint32_t dmax );
/* parse a string into tokens. for each call it will return the next token
   NOTE: does not work like the standard c function. for subsequent calls you
   have to pass the next token, not NULL */
char *strtok(char *str, char *delim);
