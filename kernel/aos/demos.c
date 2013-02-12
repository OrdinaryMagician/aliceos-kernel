/*
	demos.c : functions to show off kernel features :P.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <demos.h>
#include <types.h>
#include <helpers.h>
#include <kdefs.h>
#include <krand.h>
#include <vgatext.h>
#include <printk.h>
#include <palettes.h>
#include <vgafont.h>
#include <vgamodeset.h>
#include <vgaimgtxt.h>
#include <ramdisk.h>
#include <berp.h>
#include <vgaplanar16.h>
#include <vgamode13h.h>

demo_t demos[DEMO_COUNT] =
{
	{"blockgfx", "80x50 16-color block graphics demo", demo_blockgfx},
	{"realgfx", "basic mode 13h demo", demo_realgfx},
	{"img16", "mode 13h alice16 color set image", demo_a16gfx},
	{"img64", "mode 13h alice64 color set image", demo_a64gfx},
	{"img64g", "mode 13h alice64g color set image", demo_a64ggfx},
	{"img16c", "mode 13h custom 16 color set image", demo_c16gfx},
	{"img32c", "mode 13h custom 32 color set image", demo_c32gfx},
	{"img64c", "mode 13h custom 64 color set image", demo_c64gfx},
	{"imgfc", "mode 13h custom full (256) color set image", demo_cfullgfx},
	{"mode12", "mode 12h test (640x480 16 color)", demo_mode12},
	{"mode12imgpx", "mode 12h image blit (per-pixel)", demo_mode12imgpx},
	{"mode12imgch", "mode 12h image blit (per-channel)", demo_mode12imgch},
	{"mode12imgpx_c", "mode 12h image blit with custom palette (per-pixel)", demo_mode12imgpx_c},
	{"mode12imgch_c", "mode 12h image blit with custom palette (per-channel)", demo_mode12imgch_c},
	{"cmap", "text mode character map", demo_cmap},
};

/* list available demos */
void listdemos( void )
{
	unsigned int i;
	printk("Available demos:\n");
	for ( i=0; i<DEMO_COUNT; i++ )
		printk(" - %s : %s\n",demos[i].name,demos[i].desc);
}

/* Character map */
void demo_cmap( void )
{
	Uint16 i;
	Uint16 x,y;
	x = 4;
	y = 4;
	for ( i=0; i<128; i++ )
	{
		vga_setc(x+y*80,i,0x0F);
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
		vga_setc(x+y*80,i,0x0F);
		x++;
		if ( x >= 40 )
		{
			x = 24;
			y++;
		}
	}
}

/* mode 12h image blit (per-pixel) */
void demo_mode12imgpx( void )
{
	printk_s(SERIAL_A,"Running Mode 12h image blit demo (per-pixel)\n");
	Uint8 *aliceimg = (Uint8*)rd_find_data("alice_p16.img");
	if ( aliceimg == NULL )
		BERP("alice_p16.img not found in ramdisk");
	vga_modeset(MODE_12H);
	vga_p16init();
	vga_p16fullblit(aliceimg,BLIT_PIXEL);
}

/* mode 12h image blit (per-channel) */
void demo_mode12imgch( void )
{
	printk_s(SERIAL_A,"Running Mode 12h image blit demo (per-channel)\n");
	Uint8 *aliceimg = (Uint8*)rd_find_data("alice_p16.img");
	if ( aliceimg == NULL )
		BERP("alice_p16.img not found in ramdisk");
	vga_modeset(MODE_12H);
	vga_p16init();
	vga_p16fullblit(aliceimg,BLIT_CHANNEL);
}

/* mode 12h image blit with custom palette  (per-pixel) */
void demo_mode12imgpx_c( void )
{
	printk_s(SERIAL_A,"Running Mode 12h image blit with custom palette demo (per-pixel)\n");
	Uint8 *aliceimg = (Uint8*)rd_find_data("alice_p16c.img");
	if ( aliceimg == NULL )
		BERP("alice_p16.img not found in ramdisk");
	Uint8 *aliceimgpal = (Uint8*)rd_find_data("alice_p16c.pal");
	if ( aliceimgpal == NULL )
		BERP("alice_p16c.pal not found in ramdisk");
	vga_modeset(MODE_12H);
	vga_p16init();
	/* custom palette needs a bit of tweaking to adapt to 6-bit */
	int i;
	for ( i=0; i<48; i++ )
		alicepal256[i+APAL256_16USR_POS] = aliceimgpal[i]/4;
	vga_setpal(&alicepal256[APAL256_16USR_POS]);
	vga_p16fullblit(aliceimg,BLIT_PIXEL);
}

/* mode 12h image blit with custom palette  (per-channel) */
void demo_mode12imgch_c( void )
{
	printk_s(SERIAL_A,"Running Mode 12h image blit with custom palette demo (per-channel)\n");
	Uint8 *aliceimg = (Uint8*)rd_find_data("alice_p16c.img");
	if ( aliceimg == NULL )
		BERP("alice_p16.img not found in ramdisk");
	Uint8 *aliceimgpal = (Uint8*)rd_find_data("alice_p16c.pal");
	if ( aliceimgpal == NULL )
		BERP("alice_p16c.pal not found in ramdisk");
	vga_modeset(MODE_12H);
	vga_p16init();
	/* custom palette needs a bit of tweaking to adapt to 6-bit */
	int i;
	for ( i=0; i<48; i++ )
		alicepal256[i+APAL256_16USR_POS] = aliceimgpal[i]/4;
	vga_setpal(&alicepal256[APAL256_16USR_POS]);
	vga_p16fullblit(aliceimg,BLIT_CHANNEL);
}

/* Mode 12h (640x480 planar 16-color) */
void demo_mode12( void )
{
	printk_s(SERIAL_A,"Running Mode 12h demo\n");
	vga_modeset(MODE_12H);
	vga_p16init();
	printk_s(SERIAL_A,"Per-pixel blit (slow but steady)\n");
	vga_p16drawrect(4,4,312,472,APAL_WHITE,BLIT_PIXEL);
	printk_s(SERIAL_A,"Per-channel blit (faster but flashier)\n");
	vga_p16drawrect(324,4,312,472,APAL_WHITE,BLIT_CHANNEL);
	printk_s(SERIAL_A,"Yes, it's slow\n");
}

/* 80x50 graphics demo */
void demo_blockgfx( void )
{
	printk_s(SERIAL_A,"Running Block GFX demo\n");
	img_t aliceimg;
	if ( vga_loadimg(&aliceimg,"alice3h.img") )
		BERP("error loading alice3h.img");
	vga_initimgtxt();
	vga_fullblit(aliceimg.data);
	vga_drawrect(50,21,25,8,4);
	printk("%[4E%{51,11%s %s%{53,13%s.%s.%s%s  (%s)",_kname,_kver_code,_kver_maj,_kver_min,_kver_low,_kver_suf,_karch);
}

/* mode 13h graphics demo (full image alice16) */
void demo_a16gfx( void )
{
	printk_s(SERIAL_A,"Running Mode 13h GFX demo (full image draw on alice16 color set)\n");
	img_t aliceimg;
	if ( vga_loadimg(&aliceimg,"alice13h_16.img") )
		BERP("error loading alice13h_16.img");
	mode_13h.setmode();
	mode_13h.setpal(&alicepal256[0]);
	mode_13h.drawimg(&aliceimg,0,0,0,0,320,200,APAL256_16PAL_OFF);
}

/* mode 13h graphics demo (full image alice64) */
void demo_a64gfx( void )
{
	printk_s(SERIAL_A,"Running Mode 13h GFX demo (full image draw on alice64 color set)\n");
	img_t aliceimg;
	if ( vga_loadimg(&aliceimg,"alice13h_64.img") )
		BERP("error loading alice13h_64.img");
	mode_13h.setmode();
	mode_13h.setpal(&alicepal256[0]);
	mode_13h.drawimg(&aliceimg,0,0,0,0,320,200,APAL256_64PAL_OFF);
}

/* mode 13h graphics demo (full image alice64g) */
void demo_a64ggfx( void )
{
	printk_s(SERIAL_A,"Running Mode 13h GFX demo (full image draw on alice64g color set)\n");
	img_t aliceimg;
	if ( vga_loadimg(&aliceimg,"alice13h_64g.img") )
		BERP("error loading alice13h_64g.img");
	mode_13h.setmode();
	mode_13h.setpal(&alicepal256[0]);
	mode_13h.drawimg(&aliceimg,0,0,0,0,320,200,APAL256_64GRY_OFF);
}

/* mode 13h graphics demo (full image custom16) */
void demo_c16gfx( void )
{
	printk_s(SERIAL_A,"Running Mode 13h GFX demo (full image draw on custom 16 color set)\n");
	img_t aliceimg;
	if ( vga_loadimg(&aliceimg,"alice13h_16c.img") )
		BERP("error loading alice13h_16c.img");
	/* custom palette needs a bit of tweaking to adapt to 6-bit */
	int i;
	for ( i=0; i<48; i++ )
		alicepal256[i+APAL256_16USR_POS] = aliceimg.pal[i]/4;
	mode_13h.setmode();
	mode_13h.setpal(&alicepal256[0]);
	mode_13h.drawimg(&aliceimg,0,0,0,0,320,200,APAL256_16USR_OFF);
}

/* mode 13h graphics demo (full image custom32) */
void demo_c32gfx( void )
{
	printk_s(SERIAL_A,"Running Mode 13h GFX demo (full image draw on custom 32 color set)\n");
	img_t aliceimg;
	if ( vga_loadimg(&aliceimg,"alice13h_32c.img") )
		BERP("error loading alice13h_32c.img");
	/* custom palette needs a bit of tweaking to adapt to 6-bit */
	int i;
	for ( i=0; i<96; i++ )
		alicepal256[i+APAL256_32USR_POS] = aliceimg.pal[i]/4;
	mode_13h.setmode();
	mode_13h.setpal(&alicepal256[0]);
	mode_13h.drawimg(&aliceimg,0,0,0,0,320,200,APAL256_32USR_OFF);
}

/* mode 13h graphics demo (full image custom64) */
void demo_c64gfx( void )
{
	printk_s(SERIAL_A,"Running Mode 13h GFX demo (full image draw on custom 64 color set)\n");
	img_t aliceimg;
	if ( vga_loadimg(&aliceimg,"alice13h_64c.img") )
		BERP("error loading alice13h_64c.img");
	/* custom palette needs a bit of tweaking to adapt to 6-bit */
	int i;
	for ( i=0; i<192; i++ )
		alicepal256[i+APAL256_64USR_POS] = aliceimg.pal[i]/4;
	mode_13h.setmode();
	mode_13h.setpal(&alicepal256[0]);
	mode_13h.drawimg(&aliceimg,0,0,0,0,320,200,APAL256_64USR_OFF);
}

/* mode 13h graphics demo (full image customfull) */
void demo_cfullgfx( void )
{
	printk_s(SERIAL_A,"Running Mode 13h GFX demo (full image draw on custom 256 color set)\n");
	img_t aliceimg;
	if ( vga_loadimg(&aliceimg,"alice13h_fc.img") )
		BERP("error loading alice13h_fc.img");
	/* custom palette needs a bit of tweaking to adapt to 6-bit */
	int i;
	for ( i=0; i<768; i++ )
		aliceimg.pal[i] /= 4;
	mode_13h.setmode();
	mode_13h.setpal(&aliceimg.pal[0]);
	mode_13h.drawimg(&aliceimg,0,0,0,0,320,200,0);
}

/* mode 13h graphics demo */
void demo_realgfx( void )
{
	printk_s(SERIAL_A,"Running Mode 13h GFX demo\n");
	mode_13h.setmode();
	mode_13h.setpal(&alicepal256[0]);
	fnt_t font8;
	if ( vga_loadfnt(&font8,"fbfont8.fnt") )
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
