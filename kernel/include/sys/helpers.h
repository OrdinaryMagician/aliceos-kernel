/*
	helpers.h : Short helper functions and macros.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
/* some fundamental macros */
#define max(a,b) (((a)>(b))?(a):(b))
#define min(a,b) (((a)<(b))?(a):(b))
#define clamp(a,b,c) (((a)<(b))?(b):((a)>(c))?(c):(a))
#define abs(a) (((a)>0)?(a):((a)*(-1)))
#define bcdtodec(val) (((val)/0x10)*0x0F)+((val)&0x0F))
/* some little asm helpers */
/* cli */
extern void int_enable( void );
/* sti */
extern void int_disable( void );
/* halt kernel */
extern void khalt( uint32_t err );
/* codes for khalt (they are loaded to register eax) */
#define EXIT_BERP      0x50524542 /* basic error response printer */
#define EXIT_EXCEPTION 0xDEADBEEF /* after halt and catch fire */
#define EXIT_SUCCESS   0xADEADBED /* success return value */
