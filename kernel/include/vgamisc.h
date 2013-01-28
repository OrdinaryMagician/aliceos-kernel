/*
	vgamisc.h : miscellaneous VGA functions.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef VGAMISC_H
#define VGAMISC_H
#include <types.h>
#include <vgareg.h>

/* blank screen */
void vga_blank( void );

/* unblank screen */
void vga_unblank( void );

/* switch to 8px wide characters */
void vga_set8dot( void );

/* switch to 8px wide characters */
void vga_set9dot( void );

/* disable text blinking and allow all 16 colors for background */
void vga_disableblink( void );

/* enable text blinking reducing background colors to 8 */
void vga_enableblink( void );
#endif
