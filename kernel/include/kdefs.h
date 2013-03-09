/*
	kdefs.h : Common strings and other stuff.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef KDEFS_H
#define KDEFS_H
/* internal timer resolution */
#define TIMER_HZ 1000
/* some limits */
#define KCMDLINE_MAX 256 /* max length of kernel command line */
/* kernel stuff */
const char *_kname;
const char *_kver_fmt;
const char *_kver_maj;
const char *_kver_min;
const char *_kver_low;
const char *_kver_suf;
const char *_kver_code;
const char *_karch;
const char *_kbuild_date;
const char *_kbuild_time;
const char *_kosname;
#endif
