/*
	vgamisc.h : miscellaneous VGA functions.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef VGAMISC_H
#define VGAMISC_H
#include <sys/types.h>
/* plane switch */
void planeswitch( uint8_t p );
/* blank screen */
void vgablank( void );
/* unblank screen */
void vgaunblank( void );
/* disable text blinking and allow all 16 colors for background */
void disableblink( void );
/* enable text blinking reducing background colors to 8 */
void enableblink( void );
#endif
