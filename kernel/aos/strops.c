/*
	strops.c : "Basic" string operations.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <strops.h>
/* return lowercase of a character */
char chrlcaps( char c )
{
	/* will handle other characters later */
	return ((c>=0x41)&&(c<=0x5A))?(c+0x20):c;
}
/* convert an entire string to lowercase */
char *strlcaps( char *s )
{
	char *ret = s;
	do
		*(s) = chrlcaps(*(s));
	while ( *(++s) );
	return ret;
}
/* return CAPSLOCK'd character */
char chrcaps( char c )
{
	return ((c>=0x61)&&(c<=0x7A))?(c-0x20):c;
}
/* MAKE A STRING LOUD */
char *strcaps( char *s )
{
	char *ret = s;
	do
		*(s) = chrcaps(*(s));
	while ( *(++s) );
	return ret;
}
/* append one string to the end of another */
char *strcat( char *dest, char *src )
{
	char *ret = dest;
	while ( *(++dest) );
	while ( *src )
		*(dest++) = *(src++);
	*dest = 0;	/* obligatory null termination */
	return ret;
}
/* find first occurence of a character in a string */
char *strchr( char *in, char c )
{
	while ( *in )
	{
		if ( *in == c )
			break;
		in++;
	}
	return in;
}
/* case insensitive version of strchr */
char *strcasechr( char *in, char c )
{
	while ( *in )
	{
		if ( chrlcaps(*in) == chrlcaps(c) )
			break;
		in++;
	}
	return in;
}
/* find last occurence of a character in a string */
char *strrchr( char *in, char c )
{
	char *last = in+strlen(in);
	while ( *in )
	{
		if ( *in == c )
			last = in;
		in++;
	}
	return last;
}
/* case insensitive version of strrchr */
char *strrcasechr( char *in, char c )
{
	char *last = in+strlen(in);
	while ( *in )
	{
		if ( chrlcaps(*in) == chrlcaps(c) )
			last = in;
		in++;
	}
	return last;
}
/* compare two strings */
uint8_t strcmp( char *a, char *b )
{
	do
	{
		if ( *a != *b )
			return 1;
		if ( (!(*a) && *b) || (*a && !(*b)) )
			return 1;
	}
	while ( *(a++) && *(b++) );
	return 0;
}
/* case insensitive version of strcmp */
uint8_t strcasecmp( char *a, char *b )
{
	do
	{
		if ( chrlcaps(*a) != chrlcaps(*b) )
			return 1;
		if ( (!(*a) && *b) || (*a && !(*b)) )
			return 1;
	}
	while ( *(a++) && *(b++) );
	return 0;
}
/* overwrite one string with another */
char *strcpy( char *dest, char *src )
{
	char *ret = dest;
	while ( *src )
		*(dest++) = *(src++);
	*dest = 0;	/* obligatory null termination */
	return ret;
}
/* return if a string contains any of the characters in a set */
uint8_t strcont( char *in, char *set )
{
	while ( *in )
	{
		char *el = set;
		while ( *el )
		{
			if ( *in == *el )
				return 0;
			el++;
		}
		in++;
	}
	return 1;
}
/* case insensitive version of strcont */
uint8_t strcasecont( char *in, char *set )
{
	while ( *in )
	{
		char *el = set;
		while ( *el )
		{
			if ( chrlcaps(*in) == chrlcaps(*el) )
				return 0;
			el++;
		}
		in++;
	}
	return 1;
}
/* returns the length of a null-terminated string */
uint32_t strlen( char *s )
{
	uint32_t len = 0;
	while ( *(s++) )
		len++;
	return len;
}
/* find the first occurence of a string inside another */
char *strstr( char *in, char *s )
{
	uint32_t slen = strlen(s);
	while( *in )
	{
		if ( !strncmp(in,s,slen) )
			return in;
		in++;
	}
	return in;
}
/* case insensitive version of strstr */
char *strcasestr( char *in, char *s )
{
	uint32_t slen = strlen(s);
	while( *in )
	{
		if ( !strncasecmp(in,s,slen) )
			return in;
		in++;
	}
	return in;
}
/* find the last occurence of a string inside another */
char *strrstr( char *in, char *s )
{
	uint32_t slen = strlen(s);
	char *ret = in+strlen(in);
	while ( *in )
	{
		if ( !strncmp(in,s,slen) )
			ret = in;
		in++;
	}
	return ret;
}
/* case insensitive version of strrstr */
char *strrcasestr( char *in, char *s )
{
	uint32_t slen = strlen(s);
	char *ret = in+strlen(in);
	while ( *in )
	{
		if ( !strncasecmp(in,s,slen) )
			ret = in;
		in++;
	}
	return ret;
}
/* append a number of characters from one string to the end of another */
char *strncat( char *dest, char *src, uint32_t count )
{
	char *ret = dest;
	while ( *(++dest) );
	while ( *src && count-- )
		*(dest++) = *(src++);
	*dest = 0;	/* obligatory null termination */
	return ret;
}
/* compare a number of characters at the start of a string with another */
uint8_t strncmp( char *a, char *b, uint32_t count )
{
	do
	{
		if ( *a != *b )
			return 1;
		if ( (!(*a) && *b) || (*a && !(*b)) )
			return 1;
	}
	while ( *(a++) && *(b++) && --count );
	return 0;
}
/* case insensitive version of strncasecmp */
uint8_t strncasecmp( char *a, char *b, uint32_t count )
{
	do
	{
		if ( chrlcaps(*a) != chrlcaps(*b) )
			return 1;
		if ( (!(*a) && *b) || (*a && !(*b)) )
			return 1;
	}
	while ( *(a++) && *(b++) && --count );
	return 0;
}
/* overwrite one string with a number of characters from another */
char *strncpy( char *dest, char *src, uint32_t count )
{
	char *ret = dest;
	while ( *src && count-- )
		*(dest++) = *(src++);
	*dest = 0;	/* obligatory null termination */
	return ret;
}
/* check if a number of characters at the start of a string contains
   any of the characters in a set */
uint8_t strncont( char *in, char *set, uint32_t count )
{
	while ( *in && count-- )
	{
		char *el = set;
		while ( *el )
		{
			if ( *in != *el )
				return 1;
			el++;
		}
		in++;
	}
	return 0;
}
/* case-insensitive version of strncont */
uint8_t strncasecont( char *in, char *set, uint32_t count )
{
	while ( *in && count-- )
	{
		char *el = set;
		while ( *el )
		{
			if ( chrlcaps(*in) != chrlcaps(*el) )
				return 1;
			el++;
		}
		in++;
	}
	return 0;
}
/* find the first occurence of a string inside a number of characters at the
   beginning of a string */
char *strnstr( char *in, char *s, uint32_t count )
{
	uint32_t slen = strlen(s);
	while( *in && count-- )
	{
		if ( !strncmp(in,s,slen) )
			return in;
		in++;
	}
	return in+strlen(in);
}
/* case insensitive version of strncasestr */
char *strncasestr( char *in, char *s, uint32_t count )
{
	uint32_t slen = strlen(s);
	while( *in && count-- )
	{
		if ( !strncasecmp(in,s,slen) )
			return in;
		in++;
	}
	return in+strlen(in);
}
/* find the last occurence of a string inside a number of characters at the
   beginning of a string */
char *strrnstr( char *in, char *s, uint32_t count )
{
	uint32_t slen = strlen(s);
	char *ret = in+strlen(in);
	while ( *in && count-- )
	{
		if ( !strncmp(in,s,slen) )
			ret = in;
		in++;
	}
	return ret;
}
/* case insensitive version of strrncasestr */
char *strrncasestr( char *in, char *s, uint32_t count )
{
	uint32_t slen = strlen(s);
	char *ret = in+strlen(in);
	while ( *in && count-- )
	{
		if ( !strncasecmp(in,s,slen) )
			ret = in;
		in++;
	}
	return ret;
}
/* append one string to the end of another while keeping the length of the
   resulting string under a specific limit */
char *strlcat( char *dest, char *src, uint32_t dmax )
{
	char *ret = dest;
	while ( *(++dest) && dmax-- );
	while ( *src && dmax-- )
		*(dest++) = *(src++);
	*dest = 0;	/* obligatory null termination */
	return ret;
}
/* append a number of characters from one string to the end of another while
   keeping the length of the resulting string under a specific limit */
char *strnlcat( char *dest, char *src, uint32_t count, uint32_t dmax )
{
	char *ret = dest;
	while ( *(++dest) && dmax-- );
	while ( *src && count-- && dmax-- )
		*(dest++) = *(src++);
	*dest = 0;	/* obligatory null termination */
	return ret;
}
/* parse a string into tokens. for each call it will return the next token
   NOTE: does not work like the standard c function. for subsequent calls you
   have to pass the next token, not NULL */
char *strtok(char *str, char *delim)
{
	uint32_t len = strlen(delim);
	char *found = strstr(str,delim);
	if ( *found )
		while ( len-- )
			*(found++) = 0;
	return found;
}
