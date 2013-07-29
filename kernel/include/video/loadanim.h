/*
	loadanim.h : Animated Image loading.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
/* depth types */
#define ANM_DP_MONO   0 /* 8bpp monochrome */
#define ANM_DP_4COL   1 /* 8bpp 4 color */
#define ANM_DP_16COL  2 /* 8bpp 16 color */
#define ANM_DP_256COL 3 /* 8bpp 256 color */
#define ANM_DP_RGB8   4 /* 24bpp RGB */
#define ANM_DP_RGBA8  5 /* 32bpp RGBA */
/* flag definitions */
#define ANM_FL_NOPAL   1 /* no embedded palette */
#define ANM_FL_ONESH   2 /* one-shot (doesn't loop) */
#define ANM_FL_RESV3   4
#define ANM_FL_RESV4   8
#define ANM_FL_RESV5  16
#define ANM_FL_RESV6  32
#define ANM_FL_RESV7  64
#define ANM_FL_RESV8 128
/* struct for each anim frame */
typedef struct
{
	uint16_t index;
	uint16_t next;
	uint32_t duration;
	uint32_t frameloc;
	uint32_t framesize;
} anm_frame_t;
/* struct for an animation */
typedef struct
{
	anm_frame_t *frames;
	uint8_t *pal;
	uint16_t w;
	uint16_t h;
	uint8_t flags;
	uint8_t depth;
} anm_t;
/* load an animation from the ramdisk */
uint8_t loadanm( anm_t *dest, char *fname );
