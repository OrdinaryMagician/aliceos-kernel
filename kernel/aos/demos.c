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
#include <sys/serial.h>
#include <berp.h>
#include <fs/ramdisk.h>
#include <vga/vgapal.h>
#include <vga/vgafont.h>
#include <vga/mode13h.h>
#include <vga/mode12h.h>
#include <vga/mode3h.h>
#include <video/loadimg.h>
#include <video/loadfnt.h>
#include <video/vidtypes.h>
#include <video/fcpalette.h>
#include <bga/bga.h>
#include <sys/timer.h>
#include <sys/cmos.h>

demo_t demos[DEMO_COUNT] =
{
	{"blockgfx", "80x50 16-color block graphics demo", demo_blockgfx},
	{"crapgfx", "basic mode 12h demo", demo_crapgfx},
	{"realgfx", "basic mode 13h demo", demo_realgfx},
	{"cmap", "text mode character map", demo_cmap},
	{"bochsgfx", "Bochs/QEMU BGA demo", demo_bochsgfx},
	{"timers", "timers demo", demo_timers},
};

/* list available demos */
void listdemos( void )
{
	unsigned int i;
	mode_3h.fbputs("Available demos:\n");
	for ( i=0; i<DEMO_COUNT; i++ )
		mode_3h.fbprintf(" - %s : %s\n",demos[i].name,demos[i].desc);
}

/* update the header clock every second */
void dt_updateheader( void )
{
	/* pretty print time and date */
	Uint8 cmosval[128];
	cmos_dump(&cmosval[0]);
	char weekdays[7][4] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	mode_3h.fbsetcursor(0,0);
	mode_3h.fbsetattr(APAL_CYAN,APAL_BLUE,0);
	mode_3h.fbprintf("%{-26,0%s %02x/%02x/20%02x | %02x:%02x:%02x",weekdays[clamp(cmosval[6],1,7)-1],cmosval[7],cmosval[8],cmosval[9],cmosval[4],cmosval[2],cmosval[0]);
}

/* bottom animation */
Uint8 base_angle = 0;
void dt_wave( void )
{
	mode_3h.fbsetcursor(0,24);
	char wave[8] = {0x20, 0xB0, 0xB1, 0xB2, 0xDB, 0xB2, 0xB1, 0xB0};
	mode_3h.fbsetattr(APAL_CYAN,APAL_BLUE,EXATTR_NOSCR);
	Uint8 i;
	for ( i=0; i<80; i++ )
		mode_3h.drawchar(i,24,wave[(i+base_angle)%8]);
	base_angle++;
	if ( base_angle >= 8 )
		base_angle = 0;
}

/* bouncing ball */
Sint16 ball_x, ball_y;
Sint16 vel_x, vel_y;
void dt_bounce( void )
{
	mode_3h.drawrect(ball_x,ball_y,4,4,APAL_DARKGRAY);
	ball_x += vel_x;
	ball_y += vel_y;
	if ( ball_x > 76 )
	{
		ball_x = 76;
		vel_x = -1;
	}
	if ( ball_y > 42 )
	{
		ball_y = 42;
		vel_y = -1;
	}
	if ( ball_x < 0 )
	{
		ball_x = 0;
		vel_x = 1;
	}
	if ( ball_y < 4 )
	{
		ball_y = 4;
		vel_y = 1;
	}
	mode_3h.drawrect(ball_x,ball_y,4,4,APAL_YELLOW);
}

/* timer demo */
void demo_timers( void )
{
	/* we already assume we're in mode 3h since boot */
	mode_3h.fbcursorvis(0);
	mode_3h.fbsetattr(APAL_CYAN,APAL_BLUE,EXATTR_NOSCR);
	mode_3h.drawrect(0,4,80,42,APAL_DARKGRAY);
	printk(SERIAL_A,"Running Timers demo\n");
	ball_x = 12;
	ball_y = 8;
	vel_x = 1;
	vel_y = 1;
	int i = 0;
	while ( i < 80 )
		mode_3h.drawchar(i++,24,' ');
	/* register the tasks */
	printk(SERIAL_A,"update header, every second\n");
	if ( timer_addtask(dt_updateheader,timer_sec(1)) )
		BERP("Couldn't add task");
	printk(SERIAL_A,"bottom wave animation, every 100ms\n");
	if ( timer_addtask(dt_wave,timer_msec(100)) )
		BERP("Couldn't add task");
	printk(SERIAL_A,"bouncing ball, every 50ms\n");
	if ( timer_addtask(dt_bounce,timer_msec(50)) )
		BERP("Couldn't add task");
}

/* Character map */
void demo_cmap( void )
{
	/* no startup code, we already assume we're in mode 3h since boot */
	mode_3h.fbsetattr(15,0,EXATTR_NOSCR);
	printk(SERIAL_A,"Running Character Map demo\n");
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

/* Bochs/QEMU BGA demo */
void demo_bochsgfx( void )
{
	Uint16 rx = 1280;
	Uint16 ry = 800;
	printk(SERIAL_A,"Running BGA GFX demo\n");
	if ( bga_drv.setmode(rx,ry) )
		BERP("could not set video mode");
	fnt_t font8;
	if ( loadfnt(&font8,"fbfont8.fnt") )
		BERP("error loading fbfont8.fnt");
	fnt_t font16;
	if ( loadfnt(&font16,"fbfont16.fnt") )
		BERP("error loading fbfont16.fnt");
	bga_drv.setfont(&font16);
	bga_drv.setpal(&alicepalfc[0]);
	
	Uint16 x,y;
	int i = 0;
	/* 16 color set */
	x = 8;
	y = 8;
	while ( i < 16 )
	{
		bga_drv.drawrect(x,y,32,32,alicepalfc[i]);
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
		bga_drv.putpixel(x,y,alicepalfc[krand()&15]);
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
	bga_drv.drawimg(&aliceimg,8,80,0,0,320,200);

	img_t aliceimg2;
	if ( loadimg(&aliceimg2,"alice3h.img") )
		BERP("error loading alice3h.img");
	bga_drv.drawimg(&aliceimg2,336,80,0,0,80,50);

	/* drawing a character map would be fine too */
	bga_drv.fbsetattr(15,0,EXATTR_MASKED);
	
	/* 8x16 font */
	bga_drv.setfont(&font16);
	x = 8;
	y = 288;
	for ( i=0; i<128; i++ )
	{
		bga_drv.drawchar(x,y,i);
		x+=8;
		if ( x >= 136 )
		{
			x=8;
			y+=16;
		}
	}
	x = 144;
	y = 288;
	for ( i=128; i<256; i++ )
	{
		bga_drv.drawchar(x,y,i);
		x+=8;
		if ( x >= 272 )
		{
			x=144;
			y+=16;
		}
	}
	/* 8x8 font */
	bga_drv.setfont(&font8);
	x = 8;
	y = 424;
	for ( i=0; i<128; i++ )
	{
		bga_drv.drawchar(x,y,i);
		x+=8;
		if ( x >= 136 )
		{
			x=8;
			y+=8;
		}
	}
	x = 144;
	y = 424;
	for ( i=128; i<256; i++ )
	{
		bga_drv.drawchar(x,y,i);
		x+=8;
		if ( x >= 272 )
		{
			x=144;
			y+=8;
		}
	}

	/* let's put some cute gradients */
	x = 8;
	y = 504;
	while ( x < 264 )
	{
		bga_drv.drawvline(x,y,8,COLOR_RED(x-8));
		x++;
	}
	x = 8;
	y = 512;
	while ( x < 264 )
	{
		bga_drv.drawvline(x,y,8,COLOR_GREEN(x-8));
		x++;
	}
	x = 8;
	y = 520;
	while ( x < 264 )
	{
		bga_drv.drawvline(x,y,8,COLOR_YELLOW(x-8));
		x++;
	}
	x = 8;
	y = 528;
	while ( x < 264 )
	{
		bga_drv.drawvline(x,y,8,COLOR_BLUE(x-8));
		x++;
	}
	x = 8;
	y = 536;
	while ( x < 264 )
	{
		bga_drv.drawvline(x,y,8,COLOR_MAGENTA(x-8));
		x++;
	}
	x = 8;
	y = 544;
	while ( x < 264 )
	{
		bga_drv.drawvline(x,y,8,COLOR_CYAN(x-8));
		x++;
	}
	x = 8;
	y = 552;
	while ( x < 264 )
	{
		bga_drv.drawvline(x,y,8,COLOR_GRAY(x-8));
		x++;
	}
	
	/* noise blocks */
	x = 8;
	y = 568;
	while ( (x < 264 ) && (y < 632 ) )
	{
		bga_drv.putpixel(x,y,COLOR_RANDOM);
		x++;
		if ( x >= 264 )
		{
			x = 8;
			y++;
		}
	}
	x = 8;
	y = 640;
	while ( (x < 264 ) && (y < 704 ) )
	{
		Uint8 rval = krand();
		bga_drv.putpixel(x,y,COLOR_GRAY(rval));
		x++;
		if ( x >= 264 )
		{
			x = 8;
			y++;
		}
	}

	/* big 24bpp images (NEW) */
	img_t aliceimg3;
	if ( loadimg(&aliceimg3,"alice24bpp.img") )
		BERP("error loading alice24bpp.img");
	img_t aliceimg4;
	if ( loadimg(&aliceimg4,"mascot24bpp.img") )
		BERP("error loading mascot24bpp.img");
	/* draw the bottom image first */
	bga_drv.drawimg(&aliceimg4,280,288,0,0,480,480);
	bga_drv.drawimg(&aliceimg3,536,8,0,40,640,400);

	/* draw test strings with 8x16 font */
	bga_drv.setfont(&font16);
	bga_drv.fbsetattr(7,0,EXATTR_MASKED);

	/* colored texts */
	x = 768;
	y = 416;
	char *str = "The quick brown fox jumps over the lazy dog.";
	for ( i=0; i<8; i++ )
	{
		bga_drv.fbsetattr(i,0,EXATTR_MASKED);
		bga_drv.drawstring(x+2,y+2,str);
		bga_drv.fbsetattr(i,0,EXATTR_MASKED);
		bga_drv.drawstring(x+1,y+1,str);
		bga_drv.fbsetattr(i+8,0,EXATTR_MASKED);
		bga_drv.drawstring(x,y,str);
		y += 20;
	}
	
	/* version and stuff */
	bga_drv.fbprintf("%[E%{2,-3%s %s%{2,-2%s.%s.%s%s  (%s)",_kname,_kver_code,_kver_maj,_kver_min,_kver_low,_kver_suf,_karch);
	bga_drv.fbprintf("%[7%{-19,-2BGA graphics test");

	/* rotation doesn't really work with non-square fonts, so change the font back to 8x8 */
	bga_drv.setfont(&font8);
	bga_drv.fbsetattr(7,0,EXATTR_MASKED|EXATTR_RCW90);
	bga_drv.fbprintf("%[F%{-3,2R%{-3,3o%{-3,4t%{-3,5a%{-3,6t%{-3,7e%{-3,8d%{-3,10t%{-3,11e%{-3,12x%{-3,13t");

	/* draw some lines */
	bga_drv.drawhline(0,0,rx,COLOR_GRAY(160));
	bga_drv.drawhline(1,1,rx-2,COLOR_GRAY(255));
	bga_drv.drawhline(2,2,rx-4,COLOR_GRAY(128));
	bga_drv.drawhline(3,3,rx-6,COLOR_GRAY(96));
	bga_drv.drawhline(4,4,rx-8,COLOR_GRAY(64));

	bga_drv.drawhline(0,ry-1,rx,COLOR_GRAY(160));
	bga_drv.drawhline(1,ry-2,rx-2,COLOR_GRAY(255));
	bga_drv.drawhline(2,ry-3,rx-4,COLOR_GRAY(128));
	bga_drv.drawhline(3,ry-4,rx-6,COLOR_GRAY(96));
	bga_drv.drawhline(4,ry-5,rx-8,COLOR_GRAY(64));

	bga_drv.drawvline(0,0,ry,COLOR_GRAY(160));
	bga_drv.drawvline(1,1,ry-2,COLOR_GRAY(255));
	bga_drv.drawvline(2,2,ry-4,COLOR_GRAY(128));
	bga_drv.drawvline(3,3,ry-6,COLOR_GRAY(96));
	bga_drv.drawvline(4,4,ry-8,COLOR_GRAY(64));

	bga_drv.drawvline(rx-1,0,ry,COLOR_GRAY(160));
	bga_drv.drawvline(rx-2,1,ry-2,COLOR_GRAY(255));
	bga_drv.drawvline(rx-3,2,ry-4,COLOR_GRAY(128));
	bga_drv.drawvline(rx-4,3,ry-6,COLOR_GRAY(96));
	bga_drv.drawvline(rx-5,4,ry-8,COLOR_GRAY(64));
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
	
	/* drawing a character map would be fine too */
	mode_12h.fbsetattr(15,0,EXATTR_MASKED);
	x = 8;
	y = 288;
	for ( i=0; i<128; i++ )
	{
		mode_12h.drawchar(x,y,i);
		x+=8;
		if ( x >= 136 )
		{
			x = 8;
			y+=8;
		}
	}
	x = 144;
	y = 288;
	for ( i=128; i<256; i++ )
	{
		mode_12h.drawchar(x,y,i);
		x+=8;
		if ( x >= 272 )
		{
			x = 144;
			y+=8;
		}
	}
	
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
