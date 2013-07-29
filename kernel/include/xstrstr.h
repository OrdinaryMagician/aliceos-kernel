/*
	xstrstr.h : Some very handy C string functions.
	(C)2010-2012 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
	(This is a modified version that can be integrated into AOS)
*/
/* extended substring search */
char *xstrstr_o(char *mainstr, char *searchstr, uint32_t startoff);
char *xstrstr(char *mainstr, char *searchstr);
/* extended substring search backwards */
char *xstrlstr_o(char *mainstr, char *searchstr, uint32_t startoff);
char *xstrlstr(char *mainstr, char *searchstr);
/* extended substring search backwards (case insensitive) */
char *xstrilstr_o(char *mainstr, char *searchstr, uint32_t startoff);
char *xstrilstr(char *mainstr, char *searchstr);
/* extended substring search (case insensitive) */
char *xstristr_o(char *mainstr, char *searchstr, uint32_t startoff);
char *xstristr(char *mainstr, char *searchstr);
/* find first of for cstring */
char *xstrfof_o(char *mainstr, char *searchstr, uint32_t startoff);
char *xstrfof(char *mainstr, char *searchstr);
/* find first of for cstring (case insensitive!) */
char *xstrifof_o(char *mainstr, char *searchstr, uint32_t startoff);
char *xstrifof(char *mainstr, char *searchstr);
/* find first NOT of for cstring */
char *xstrfnof_o(char *mainstr, char *searchstr, uint32_t startoff);
char *xstrfnof(char *mainstr, char *searchstr);
/* find first NOT of for cstring (case insensitive!) */
char *xstrifnof_o(char *mainstr, char *searchstr, uint32_t startoff);
char *xstrifnof(char *mainstr, char *searchstr);
/* find last of for cstring */
char *xstrlof_o(char *mainstr, char *searchstr, uint32_t startoff);
char *xstrlof(char *mainstr, char *searchstr);
/* find last of for cstring (case insensitive!) */
char *xstrilof_o(char *mainstr, char *searchstr, uint32_t startoff);
char *xstrilof(char *mainstr, char *searchstr);
/* find last NOT of for cstring */
char *xstrlnof_o(char *mainstr, char *searchstr, uint32_t startoff);
char *xstrlnof(char *mainstr, char *searchstr);
/* find last NOT of for cstring (case insensitive!) */
char *xstrilnof_o(char *mainstr, char *searchstr, uint32_t startoff);
char *xstrilnof(char *mainstr, char *searchstr);
