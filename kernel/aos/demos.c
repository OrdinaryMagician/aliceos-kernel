/*
	demos.c : functions to show off kernel features :P.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <demos.h>
#include <sys/types.h>
#include <sys/helpers.h>
#include <kdefs.h>
#include <krand.h>
#include <printk.h>
#include <serial.h>
#include <berp.h>
#include <fs/ramdisk.h>
#include <vga/palettes.h>
#include <vga/font.h>
#include <vga/mode13h.h>
#include <vga/mode12h.h>
#include <vga/mode3h.h>

demo_t demos[DEMO_COUNT] =
{
	{"blockgfx", "80x50 16-color block graphics demo", demo_blockgfx},
	{"crapgfx", "basic mode 12h demo", demo_crapgfx},
	{"realgfx", "basic mode 13h demo", demo_realgfx},
	{"cmap", "text mode character map", demo_cmap},
};

/* list available demos */
void listdemos( void )
{
	unsigned int i;
	mode_3h.fbputs("Available demos:\n");
	for ( i=0; i<DEMO_COUNT; i++ )
		mode_3h.fbprintf(" - %s : %s\n",demos[i].name,demos[i].desc);
}

/* Character map */
void demo_cmap( void )
{
	/* no startup code, we already assume we're in mode 3h since boot */
	mode_3h.fbsetattr(15,0,EXATTR_NOSCR);
	Uint16 i;
	Uint16 x,y;
	x = 4;
	y = 4;
	for ( i=0; i<128; i++ )
	{
		mode_3h.drawchar(x,y,i);
		x++;
		if ( x >= 20 )
		{
			x = 4;
			y++;
		}
	}
	x = 24;
	y = 4;
	for ( i=128; i<256; i++ )
	{
		mode_3h.drawchar(x,y,i);
		x++;
		if ( x >= 40 )
		{
			x = 24;
			y++;
		}
	}
}

/* 80x50 graphics demo */
void demo_blockgfx( void )
{
	/* no startup code, we already assume we're in mode 3h since boot */
	mode_3h.fbsetattr(15,0,EXATTR_NOSCR);
	printk(SERIAL_A,"Running Block GFX demo\n");
	img_t aliceimg;
	if ( loadimg(&aliceimg,"alice3h.img") )
		BERP("error loading alice3h.img");
	mode_3h.clear();
	mode_3h.drawimg(&aliceimg,0,0,0,0,80,50,0);
	mode_3h.drawrect(50,21,25,8,4);
	mode_3h.fbprintf("%[4E%{51,11%s %s%{51,13%s.%s.%s%s (%s)",_kname,_kver_code,_kver_maj,_kver_min,_kver_low,_kver_suf,_karch);
	mode_3h.drawhline(0,0,80,7);
	mode_3h.drawhline(0,49,80,7);
	mode_3h.drawvline(0,0,50,7);
	mode_3h.drawvline(79,0,50,7);
}

/* mode 12h graphics demo */
void demo_crapgfx( void )
{
	printk(SERIAL_A,"Running Mode 12h GFX demo\n");
	mode_12h.setmode();
	mode_12h.setpal(&alicepal[0]);
	fnt_t font8;
	if ( loadfnt(&font8,"fbfont8.fnt") )
		BERP("error loading fbfont8.fnt");
	mode_12h.setfont(&font8);
	Uint16 x,y;
	int i = 0;
	/* 16 color set */
	x = 8;
	y = 8;
	while ( i < 16 )
	{
		mode_12h.drawrect(x,y,32,32,i);
		x+=32;
		if ( !((++i)%8) )
		{
			x=8;
			y+=32;
		}
	}
	
	/* draw some colorful noise */
	x = 272;
	y = 8;
	do
	{
		mode_12h.putpixel(x,y,krand()%16);
		x++;
		if ( x >= 528 )
		{
			x = 272;
			y++;
		}
	}
	while ( (x < 528) && (y < 72) );
	
	/* there's enough space to draw this */
	img_t aliceimg;
	if ( loadimg(&aliceimg,"alice13h_16.img") )
		BERP("error loading alice13h_16.img");
	mode_12h.drawimg(&aliceimg,8,80,0,0,320,200,0);
	
	img_t aliceimg2;
	if ( loadimg(&aliceimg2,"alice3h.img") )
		BERP("error loading alice3h.img");
	mode_12h.drawimg(&aliceimg2,336,80,0,0,80,50,0);

	/* draw test strings */
	mode_12h.fbsetattr(7,0,EXATTR_MASKED);
	mode_12h.fbprintf("%[E%{1,-3%s %s%{1,-2%s.%s.%s%s  (%s)",_kname,_kver_code,_kver_maj,_kver_min,_kver_low,_kver_suf,_karch);
	mode_12h.fbprintf("%[7%{-14,-2Mode 12h test");
	mode_12h.fbsetattr(7,0,EXATTR_MASKED|EXATTR_RCW90);
	mode_12h.fbprintf("%[F%{-3,2R%{-3,3o%{-3,4t%{-3,5a%{-3,6t%{-3,7e%{-3,8d%{-3,10t%{-3,11e%{-3,12x%{-3,13t");
	
	/* draw some lines */
	mode_12h.drawhline(0,0,640,8);
	mode_12h.drawhline(1,1,638,7);
	mode_12h.drawhline(2,2,636,15);
	mode_12h.drawhline(3,3,634,7);
	mode_12h.drawhline(4,4,632,8);
	
	mode_12h.drawhline(0,479,640,8);
	mode_12h.drawhline(1,478,638,7);
	mode_12h.drawhline(2,477,636,15);
	mode_12h.drawhline(3,476,634,7);
	mode_12h.drawhline(4,475,632,8);
	
	mode_12h.drawvline(0,0,480,8);
	mode_12h.drawvline(1,1,478,7);
	mode_12h.drawvline(2,2,476,15);
	mode_12h.drawvline(3,3,474,7);
	mode_12h.drawvline(4,4,472,8);
	
	mode_12h.drawvline(639,0,480,8);
	mode_12h.drawvline(638,1,478,7);
	mode_12h.drawvline(637,2,476,15);
	mode_12h.drawvline(636,3,474,7);
	mode_12h.drawvline(635,4,472,8);
}

/* mode 13h graphics demo */
void demo_realgfx( void )
{
	printk(SERIAL_A,"Running Mode 13h GFX demo\n");
	mode_13h.setmode();
	/* needed for special "masking" */
	alicepal256[765] = 0;
	alicepal256[766] = 0;
	alicepal256[767] = 0;
	mode_13h.setpal(&alicepal256[0]);
	fnt_t font8;
	if ( loadfnt(&font8,"fbfont8.fnt") )
		BERP("error loading fbfont8.fnt");
	mode_13h.setfont(&font8);
	Uint16 x,y;
	int i = 0;
	/* 16 color set */
	x = 8;
	y = 8;
	while ( i < 16 )
	{
		mode_13h.drawrect(x,y,8,8,i);
		x+=8;
		if ( !((++i)%8) )
		{
			x=8;
			y+=8;
		}
	}
	/* 64 color set */
	y += 8;
	while ( i < 80 )
	{
		mode_13h.drawrect(x,y,8,8,i);
		x+=8;
		if ( !((++i)%8) )
		{
			x=8;
			y+=8;
		}
	}
	/* 64 gray set */
	y += 8;
	while ( i < 144 )
	{
		mode_13h.drawrect(x,y,8,8,i);
		x+=8;
		if ( !((++i)%8) )
		{
			x=8;
			y+=8;
		}
	}
	/* user palettes left out */
	
	/* draw some colorful noise (using each subpalette) */
	x = 80;
	y = 8;
	do
	{
		mode_13h.putpixel(x,y,krand()%16);
		x++;
		if ( x >= 144 )
		{
			x = 80;
			y++;
		}
	}
	while ( (x < 144) && (y < 24) );
	x = 80;
	y = 32;
	do
	{
		mode_13h.putpixel(x,y,16+(krand()%64));
		x++;
		if ( x >= 144 )
		{
			x = 80;
			y++;
		}
	}
	while ( (x < 144) && (y < 96) );
	while ( (x < 144) && (y < 24) );
	x = 80;
	y = 104;
	do
	{
		mode_13h.putpixel(x,y,80+(krand()%64));
		x++;
		if ( x >= 144 )
		{
			x = 80;
			y++;
		}
	}
	while ( (x < 144) && (y < 168) );

	/* draw some gradients using the 64 color subpalettes */
	Uint32 val;
	val = 0;
	x = 152;
	y = 8;
	do
	{
		mode_13h.putpixel(x,y,16+(val/32));
		val++;
		x++;
		if ( x >= 280 )
		{
			x = 152;
			y++;
			val = 0;
		}
	}
	while ( (x < 280) && (y < 24) );
	val = 0;
	x = 152;
	y = 24;
	do
	{
		mode_13h.putpixel(x,y,16+(val/32)*4);
		val++;
		x++;
		if ( x >= 280 )
		{
			x = 152;
			y++;
			val = 0;
		}
	}
	while ( (x < 280) && (y < 40) );
	val = 0;
	x = 152;
	y = 40;
	do
	{
		mode_13h.putpixel(x,y,16+(val/32)*16);
		val++;
		x++;
		if ( x >= 280 )
		{
			x = 152;
			y++;
			val = 0;
		}
	}
	while ( (x < 280) && (y < 56) );
	val = 0;
	x = 152;
	y = 56;
	do
	{
		mode_13h.putpixel(x,y,80+(val/2));
		val++;
		x++;
		if ( x >= 280 )
		{
			x = 152;
			y++;
			val = 0;
		}
	}
	while ( (x < 280) && (y < 72) );
	
	/* enough space to draw this */
	img_t aliceimg;
	if ( loadimg(&aliceimg,"alice3h.img") )
		BERP("error loading alice3h.img");
	mode_13h.drawimg(&aliceimg,152,80,0,0,80,50,0);

	/* draw test strings */
	mode_13h.fbsetattr(7,0,EXATTR_MASKED);
	mode_13h.fbprintf("%[E%{1,-3%s %s%{1,-2%s.%s.%s%s  (%s)",_kname,_kver_code,_kver_maj,_kver_min,_kver_low,_kver_suf,_karch);
	mode_13h.fbprintf("%[7%{-14,-2Mode 13h test");
	mode_13h.fbsetattr(7,0,EXATTR_MASKED|EXATTR_RCW90);
	mode_13h.fbprintf("%[F%{-3,2R%{-3,3o%{-3,4t%{-3,5a%{-3,6t%{-3,7e%{-3,8d%{-3,10t%{-3,11e%{-3,12x%{-3,13t");
	
	/* draw some lines */
	mode_13h.drawhline(0,0,320,8);
	mode_13h.drawhline(1,1,318,7);
	mode_13h.drawhline(2,2,316,15);
	mode_13h.drawhline(3,3,314,7);
	mode_13h.drawhline(4,4,312,8);
	
	mode_13h.drawhline(0,199,320,8);
	mode_13h.drawhline(1,198,318,7);
	mode_13h.drawhline(2,197,316,15);
	mode_13h.drawhline(3,196,314,7);
	mode_13h.drawhline(4,195,312,8);
	
	mode_13h.drawvline(0,0,200,8);
	mode_13h.drawvline(1,1,198,7);
	mode_13h.drawvline(2,2,196,15);
	mode_13h.drawvline(3,3,194,7);
	mode_13h.drawvline(4,4,192,8);
	
	mode_13h.drawvline(319,0,200,8);
	mode_13h.drawvline(318,1,198,7);
	mode_13h.drawvline(317,2,196,15);
	mode_13h.drawvline(316,3,194,7);
	mode_13h.drawvline(315,4,192,8);
}
