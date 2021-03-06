/*
	vidtypes.h : Definitions for certain video data types.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
typedef struct
{
	uint8_t a, r, g, b;
} color_t;
/* color in RGB (alpha always 255) */
#define color3(r,g,b) color(r,g,b,255)
/* predefined color macros */
#define COLOR_GRAY(i)    color3(i,i,i)
#define COLOR_RED(i)     color3(i,0,0)
#define COLOR_GREEN(i)   color3(0,i,0)
#define COLOR_BLUE(i)    color3(0,0,i)
#define COLOR_CYAN(i)    color3(0,i,i)
#define COLOR_MAGENTA(i) color3(i,0,i)
#define COLOR_YELLOW(i)  color3(i,i,0)
#define COLOR_RANDOM     color3(krand(),krand(),krand())
/* NULL color */
#define COLOR_NULL       color(0,0,0,0)
/* easy color_t creation */
color_t color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
