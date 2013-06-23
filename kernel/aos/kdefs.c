/*
	kdefs.c : Common strings and other stuff.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <kdefs.h>
/* kernel stuff */
const char *_kname = "AliceOS kernel";
const char *_kver_fmt = "%d.%d.%d%s";
const char *_kver_maj = "0";
const char *_kver_min = "0";
const char *_kver_low = "4";
const char *_kver_suf = "a";
const char *_kver_code = "Shanghai";
#if AOSX86
const char *_karch = "i486";
#elif AOSX64
const char *_karch = "x86_64";
#elif AOSRPI
const char *_karch = "ARM";
#elif AOSDS2
const char *_karch = "MIPS";
#else
const char *_karch = "Unknown";
#endif
const char *_kbuild_date = __DATE__;
const char *_kbuild_time = __TIME__;
const char *_kosname = "AOS";
