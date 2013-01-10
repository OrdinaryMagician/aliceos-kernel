/*
	strops.c : "Basic" string operations.
	(C)2012 Marisa Kirisame, UnSX Team.
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
	while ( *s )
	{
		*(s) = chrlcaps(*s);
		s++;
	}
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
	while ( *s )
	{
		*(s) = chrcaps(*s);
		s++;
	}
	return ret;
}

/* append one string to the end of another */
char *strcat( char *dest, char *src )
{
	char *ret = dest;
	while ( *(dest++) );
	dest--;
	while ( *src )
		*(dest++) = *(src++);
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
Uint8 strcmp( char *a, char *b )
{
	do
	{
		if ( *a != *b )
			return 1;
		if ( (!(*a) && *b) || (*a && !(*b)) )
			return 1;
		a++;
		b++;
	}
	while ( *a && *b );
	return 0;
}

/* case insensitive version of strcmp */
Uint8 strcasecmp( char *a, char *b )
{
	do
	{
		if ( chrlcaps(*a) != chrlcaps(*b) )
			return 1;
		if ( (!(*a) && *b) || (*a && !(*b)) )
			return 1;
		a++;
		b++;
	}
	while ( *a && *b );
	return 0;
}

/* overwrite one string with another */
char *strcpy( char *dest, char *src )
{
	char *ret = dest;
	while ( *src )
		*(dest++) = *(src++);
	return ret;
}

/* return if a string contains any of the characters in a set */
Uint8 strcont( char *in, char *set )
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
Uint8 strcasecont( char *in, char *set )
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
Uint32 strlen( char *s )
{
	Uint32 len = 0;
	while ( *(s++) )
		len++;
	return len;
}

/* find the first occurence of a string inside another */
char *strstr( char *in, char *s )
{
	Uint32 slen = strlen(s);
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
	Uint32 slen = strlen(s);
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
	Uint32 slen = strlen(s);
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
	Uint32 slen = strlen(s);
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
char *strncat( char *dest, char *src, Uint32 count )
{
	char *ret = dest;
	while ( *(dest++) );
	dest--;
	while ( *src && count-- )
		*(dest++) = *(src++);
	*dest = 0;	/* obligatory null termination */
	return ret;
}

/* compare a number of characters at the start of a string with another */
Uint8 strncmp( char *a, char *b, Uint32 count )
{
	do
	{
		if ( *a != *b )
			return 1;
		if ( (!(*a) && *b) || (*a && !(*b)) )
			return 1;
		a++;
		b++;
	}
	while ( *a && *b && count-- );
	return 0;
}

/* case insensitive version of strncasecmp */
Uint8 strncasecmp( char *a, char *b, Uint32 count )
{
	do
	{
		if ( chrlcaps(*a) != chrlcaps(*b) )
			return 1;
		if ( (!(*a) && *b) || (*a && !(*b)) )
			return 1;
		a++;
		b++;
	}
	while ( *a && *b && count-- );
	return 0;
}

/* overwrite one string with a number of characters from another */
char *strncpy( char *dest, char *src, Uint32 count )
{
	char *ret = dest;
	while ( *src && count-- )
		*(dest++) = *(src++);
	*dest = 0;	/* obligatory null termination */
	return ret;
}

/* check if a number of characters at the start of a string contains
   any of the characters in a set */
Uint8 strncont( char *in, char *set, Uint32 count )
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
Uint8 strncasecont( char *in, char *set, Uint32 count )
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
char *strnstr( char *in, char *s, Uint32 count )
{
	Uint32 slen = strlen(s);
	while( *in && count-- )
	{
		if ( !strncmp(in,s,slen) )
			return in;
		in++;
	}
	return in+strlen(in);
}

/* case insensitive version of strncasestr */
char *strncasestr( char *in, char *s, Uint32 count )
{
	Uint32 slen = strlen(s);
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
char *strrnstr( char *in, char *s, Uint32 count )
{
	Uint32 slen = strlen(s);
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
char *strrncasestr( char *in, char *s, Uint32 count )
{
	Uint32 slen = strlen(s);
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
char *strlcat( char *dest, char *src, Uint32 dmax )
{
	char *ret = dest;
	while ( *(dest++) && dmax-- );
	dest--;
	dmax--;
	while ( *src && dmax-- )
		*(dest++) = *(src++);
	*dest = 0;	/* obligatory null termination */
	return ret;
}

/* append a number of characters from one string to the end of another while
   keeping the length of the resulting string under a specific limit */
char *strnlcat( char *dest, char *src, Uint32 count, Uint32 dmax )
{
	char *ret = dest;
	while ( *(dest++) && dmax-- );
	dest--;
	dmax--;
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
	Uint32 len = strlen(delim);
	char *found = strstr(str,delim);
	if ( *found )
		while ( len-- )
			*(found++) = 0;
	return found;
}
