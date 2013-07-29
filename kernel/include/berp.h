/*
	berp.h : Unrecoverable error handling.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#define BERP(msg) berp(msg,__FILE__,__LINE__);
void berp( const char *message, const char *file, uint32_t line );
