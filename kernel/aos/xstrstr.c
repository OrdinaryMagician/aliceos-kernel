/*
	xstrstr.c : Some very handy C string functions.
	(C)2010-2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <xstrstr.h>
#include <strops.h>
/* extended substring search */
char *xstrstr_o(char *mainstr, char *searchstr, uint32_t startoff)
{
	uint32_t posz = 0, lent = 0;
	uint8_t foundit = 0;
	if ( strlen(mainstr) < strlen(searchstr) )
		return NULL;
	if ( strlen(searchstr) <= 0 )
		return NULL;
	if ( (strlen(mainstr)-startoff) <= 0 )
		return NULL;
	if ( strlen(searchstr) > (strlen(mainstr)-startoff) )
		return NULL;
	lent = (strlen(mainstr)-startoff)-strlen(searchstr);
	do
	{
		if ( !strncmp((mainstr+posz+startoff),searchstr,strlen(searchstr)) )
			foundit = 1;
		else
			posz++;
	}
	while ( (posz <= lent) && !foundit );
	if ( foundit )
		return (mainstr+posz+startoff);
	else
		return NULL;
}
char *xstrstr(char *mainstr, char *searchstr)
{
	return xstrstr_o(mainstr,searchstr,0);
}
/* extended substring search backwards */
char *xstrlstr_o(char *mainstr, char *searchstr, uint32_t startoff)
{
	uint32_t posz = 0;
	uint8_t foundit = 0;
	if ( strlen(mainstr) < strlen(searchstr) )
		return NULL;
	if ( strlen(searchstr) <= 0 )
		return NULL;
	if ( startoff > (strlen(mainstr)-strlen(searchstr)) )
		startoff = strlen(mainstr)-strlen(searchstr);
	if ( (strlen(mainstr)-startoff) <= 0 )
		return NULL;
	if ( strlen(searchstr) > (strlen(mainstr)-startoff) )
		return NULL;
	posz = startoff;
	do
	{
		if ( !strncmp((mainstr+posz),searchstr,strlen(searchstr)) )
			foundit = 1;
		else
			posz--;
	}
	while ( (posz > 0) && !foundit );
	if ( foundit )
		return (mainstr+posz);
	else
		return NULL;
}
char *xstrlstr(char *mainstr, char *searchstr)
{
	return xstrlstr_o(mainstr,searchstr,strlen(mainstr));
}
/* extended substring search backwards (case insensitive) */
char *xstrilstr_o(char *mainstr, char *searchstr, uint32_t startoff)
{
	uint32_t posz = 0;
	uint8_t foundit = 0;
	if ( strlen(mainstr) < strlen(searchstr) )
		return NULL;
	if ( strlen(searchstr) <= 0 )
		return NULL;
	if ( startoff > (strlen(mainstr)-strlen(searchstr)) )
		startoff = strlen(mainstr)-strlen(searchstr);
	if ( (strlen(mainstr)-startoff) <= 0 )
		return NULL;
	if ( strlen(searchstr) > (strlen(mainstr)-startoff) )
		return NULL;
	posz = startoff;
	do
	{
		if ( !strncasecmp((mainstr+posz),searchstr,strlen(searchstr)) )
			foundit = 1;
		else
			posz--;
	}
	while ( (posz > 0) && !foundit );
	if ( foundit )
		return (mainstr+posz);
	else
		return NULL;
}
char *xstrilstr(char *mainstr, char *searchstr)
{
	return xstrilstr_o(mainstr,searchstr,strlen(mainstr));
}
/* extended substring search (case insensitive) */
char *xstristr_o(char *mainstr, char *searchstr, uint32_t startoff)
{
	uint32_t posz = 0, lent = 0;
	uint8_t foundit = 0;
	if ( strlen(mainstr) < strlen(searchstr) )
		return NULL;
	if ( strlen(searchstr) <= 0 )
		return NULL;
	if ( startoff > (strlen(mainstr)-strlen(searchstr)) )
		startoff = strlen(mainstr)-strlen(searchstr);
	if ( (strlen(mainstr)-startoff) <= 0 )
		return NULL;
	if ( strlen(searchstr) > (strlen(mainstr)-startoff) )
		return NULL;
	lent = (strlen(mainstr)-startoff)-strlen(searchstr);
	do
	{
		if ( !strncasecmp((mainstr+posz+startoff),searchstr,strlen(searchstr)) )
			foundit = 1;
		else
			posz++;
	}
	while ( (posz <= lent) && !foundit );
	if ( foundit )
		return (mainstr+posz+startoff);
	else
		return NULL;
}
char *xstristr(char *mainstr, char *searchstr)
{
	return xstristr_o(mainstr,searchstr,0);
}
/* find first of for cstring */
char *xstrfof_o(char *mainstr, char *searchstr, uint32_t startoff)
{
	uint32_t posz = 0, sposz = 0, lent = 0;
	uint8_t foundit = 0;
	if ( strlen(searchstr) <= 0 )
		return NULL;
	if ( strlen(mainstr) <= 0 )
		return NULL;
	if ( startoff > (strlen(mainstr)-1) )
		startoff = strlen(mainstr)-1;
	lent = (strlen(mainstr)-startoff);
	do
	{
		for ( sposz = 0; sposz <= strlen(searchstr); sposz++ )
		{
			if ( foundit )
				break;
			if ( !strncmp((mainstr+posz+startoff),(searchstr+sposz),1) )
				foundit = 1;
		}
		if ( !foundit )
			posz++;
	}
	while ( (posz <= lent) && !foundit );
	if ( foundit )
		return (mainstr+posz+startoff);
	else
		return NULL;
}
char *xstrfof(char *mainstr, char *searchstr)
{
	return xstrfof_o(mainstr,searchstr,0);
}
/* find first of for cstring (case insensitive!) */
char *xstrifof_o(char *mainstr, char *searchstr, uint32_t startoff)
{
	uint32_t posz = 0, sposz = 0, lent = 0;
	uint8_t foundit = 0;
	if ( strlen(searchstr) <= 0 )
		return NULL;
	if ( strlen(mainstr) <= 0 )
		return NULL;
	if ( startoff > (strlen(mainstr)-1) )
		startoff = strlen(mainstr)-1;
	lent = (strlen(mainstr)-startoff);
	do
	{
		for ( sposz = 0; sposz <= strlen(searchstr); sposz++ )
		{
			if ( foundit )
				break;
			if ( !strncasecmp((mainstr+posz+startoff),(searchstr+sposz),1) )
				foundit = 1;
		}
		if ( !foundit )
			posz++;
	}
	while ( (posz <= lent) && !foundit );
	if ( foundit )
		return (mainstr+posz+startoff);
	else
		return NULL;
}
char *xstrifof(char *mainstr, char *searchstr)
{
	return xstrifof_o(mainstr,searchstr,0);
}
/* find first NOT of for cstring */
char *xstrfnof_o(char *mainstr, char *searchstr, uint32_t startoff)
{
	uint32_t posz = 0, sposz = 0, lent = 0;
	uint32_t occurences = 0;
	if ( strlen(searchstr) <= 0 )
		return NULL;
	if ( strlen(mainstr) <= 0 )
		return NULL;
	if ( startoff > (strlen(mainstr)-1) )
		startoff = strlen(mainstr)-1;
	lent = (strlen(mainstr)-startoff);
	do
	{
		occurences = 0;
		for ( sposz = 0; sposz <= strlen(searchstr); sposz++ )
			if ( !strncmp((mainstr+posz+startoff),(searchstr+sposz),1) )
				occurences++;
		if ( occurences != 0 )
			posz++;
	}
	while ( (posz <= lent) && (occurences != 0) );
	if ( occurences == 0 )
		return (mainstr+posz+startoff);
	else
		return NULL;
}
char *xstrfnof(char *mainstr, char *searchstr)
{
	return xstrfnof_o(mainstr,searchstr,0);
}
/* find first NOT of for cstring (case insensitive!) */
char *xstrifnof_o(char *mainstr, char *searchstr, uint32_t startoff)
{
	uint32_t posz = 0, sposz = 0, lent = 0;
	uint32_t occurences = 0;
	if ( strlen(searchstr) <= 0 )
		return NULL;
	if ( strlen(mainstr) <= 0 )
		return NULL;
	if ( startoff > (strlen(mainstr)-1) )
		startoff = strlen(mainstr)-1;
	lent = (strlen(mainstr)-startoff);
	do
	{
		occurences = 0;
		for ( sposz = 0; sposz <= strlen(searchstr); sposz++ )
			if ( !strncasecmp((mainstr+posz+startoff),(searchstr+sposz),1) )
				occurences++;
		if ( occurences != 0 )
			posz++;
	}
	while ( (posz <= lent) && (occurences != 0) );
	if ( occurences == 0 )
		return (mainstr+posz+startoff);
	else
		return NULL;
}
char *xstrifnof(char *mainstr, char *searchstr)
{
	return xstrifnof_o(mainstr,searchstr,0);
}
/* find last of for cstring */
char *xstrlof_o(char *mainstr, char *searchstr, uint32_t startoff)
{
	uint32_t posz = 0, sposz = 0;
	uint8_t foundit = 0;
	if ( strlen(searchstr) <= 0 )
		return NULL;
	if ( strlen(mainstr) <= 0 )
		return NULL;
	if ( startoff > (strlen(mainstr)-1) )
		startoff = strlen(mainstr)-1;
	posz = startoff;
	do
	{
		for ( sposz = 0; sposz <= strlen(searchstr); sposz++ )
		{
			if ( foundit )
				break;
			if ( !strncmp((mainstr+posz),(searchstr+sposz),1) )
				foundit = 1;
		}
		if ( !foundit )
			posz--;
	}
	while ( (posz > 0) && !foundit );
	if ( foundit )
		return (mainstr+posz);
	else
		return NULL;
}
char *xstrlof(char *mainstr, char *searchstr)
{
	return xstrlof_o(mainstr,searchstr,strlen(mainstr));
}
/* find last of for cstring (case insensitive!) */
char *xstrilof_o(char *mainstr, char *searchstr, uint32_t startoff)
{
	uint32_t posz = 0, sposz = 0;
	uint8_t foundit = 0;
	if ( strlen(searchstr) <= 0 )
		return NULL;
	if ( strlen(mainstr) <= 0 )
		return NULL;
	if ( startoff > (strlen(mainstr)-1) )
		startoff = strlen(mainstr)-1;
	posz = startoff;
	do
	{
		for ( sposz = 0; sposz <= strlen(searchstr); sposz++ )
		{
			if ( foundit )
				break;
			if ( !strncasecmp((mainstr+posz),(searchstr+sposz),1) )
				foundit = 1;
		}
		if ( !foundit )
			posz--;
	}
	while ( (posz > 0) && !foundit );
	if ( foundit )
		return (mainstr+posz);
	else
		return NULL;
}
char *xstrilof(char *mainstr, char *searchstr)
{
	return xstrilof_o(mainstr,searchstr,strlen(mainstr));
}
/* find last NOT of for cstring */
char *xstrlnof_o(char *mainstr, char *searchstr, uint32_t startoff)
{
	uint32_t posz = 0, sposz = 0;
	uint32_t occurences = 0;
	if ( strlen(searchstr) <= 0 )
		return NULL;
	if ( strlen(mainstr) <= 0 )
		return NULL;
	if ( startoff > (strlen(mainstr)-1) )
		startoff = strlen(mainstr)-1;
	posz = startoff;
	do
	{
		occurences = 0;
		for ( sposz = 0; sposz <= strlen(searchstr); sposz++ )
			if ( !strncmp((mainstr+posz),(searchstr+sposz),1) )
				occurences++;
		if ( occurences != 0 )
			posz--;
	}
	while ( (posz > 0) && (occurences != 0) );
	if ( occurences == 0 )
		return (mainstr+posz);
	else
		return NULL;
}
char *xstrlnof(char *mainstr, char *searchstr)
{
	return xstrlnof_o(mainstr,searchstr,strlen(mainstr));
}
/* find last NOT of for cstring (case insensitive!) */
char *xstrilnof_o(char *mainstr, char *searchstr, uint32_t startoff)
{
	uint32_t posz = 0, sposz = 0;
	uint32_t occurences = 0;
	if ( strlen(searchstr) <= 0 )
		return NULL;
	if ( strlen(mainstr) <= 0 )
		return NULL;
	if ( startoff > (strlen(mainstr)-1) )
		startoff = strlen(mainstr)-1;
	posz = startoff;
	do
	{
		occurences = 0;
		for ( sposz = 0; sposz <= strlen(searchstr); sposz++ )
			if ( !strncasecmp((mainstr+posz),(searchstr+sposz),1) )
				occurences++;
		if ( occurences != 0 )
			posz--;
	}
	while ( (posz > 0) && (occurences != 0) );
	if ( occurences == 0 )
		return (mainstr+posz);
	else
		return NULL;
}
char *xstrilnof(char *mainstr, char *searchstr)
{
	return xstrilnof_o(mainstr,searchstr,strlen(mainstr));
}
