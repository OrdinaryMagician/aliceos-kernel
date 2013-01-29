/*
	demos.c : functions to show off kernel features :P.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <demos.h>

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

/* Mode 12h (640x480 planar 16-color) */
void demo_mode12( void )
{
	printk_s(SERIAL_A,"Running Mode 12h demo\n");
	vga_modeset(MODE_12H);
	vga_p16init();
	Uint32 i = 0, j = 0;
	for ( i=0; i<480; i++ )
		for ( j=0; j<640; j++ )
			vga_p16putpixel(j,i,krand());
	printk_s(SERIAL_A,"Yup, it's THAT slow\n");
}

/* 80x50 graphics demo */
void demo_blockgfx( void )
{
	printk_s(SERIAL_A,"Running Block GFX demo\n");
	Uint8 *aliceimg = (Uint8*)rd_find_data("alice_txt.img");
	if ( aliceimg == NULL )
		BERP("alice_txt.img not found in ramdisk");
	vga_initimgtxt();
	vga_fullblit(aliceimg);
	vga_drawrect(50,21,25,8,4);
	/* splash screen */
	printk("%[4E%{51,11%s %s%{53,13%s.%s.%s-%s  (%s)",_kname,_kver_code,_kver_maj,_kver_min,_kver_low,_kver_suf,_karch);
}

/* mode 13h graphics demo (full image alice16) */
void demo_a16gfx( void )
{
	printk_s(SERIAL_A,"Running Mode 13h GFX demo (full image draw on alice16 color set)\n");
	Uint8 *aliceimg = (Uint8*)rd_find_data("alice_13h_16.img");
	if ( aliceimg == NULL )
		BERP("alice_13h_16.img not found in ramdisk");
	vga_modeset(MODE_13H);
	vga_13hsetpal(&alicepal256[0]);
	vga_13hfullblit(aliceimg,APAL256_16PAL_OFF);
}

/* mode 13h graphics demo (full image alice64) */
void demo_a64gfx( void )
{
	printk_s(SERIAL_A,"Running Mode 13h GFX demo (full image draw on alice64 color set)\n");
	Uint8 *aliceimg = (Uint8*)rd_find_data("alice_13h_64.img");
	if ( aliceimg == NULL )
		BERP("alice_13h_64.img not found in ramdisk");
	vga_modeset(MODE_13H);
	vga_13hsetpal(&alicepal256[0]);
	vga_13hfullblit(aliceimg,APAL256_64PAL_OFF);
}

/* mode 13h graphics demo (full image alice64g) */
void demo_a64ggfx( void )
{
	printk_s(SERIAL_A,"Running Mode 13h GFX demo (full image draw on alice64g color set)\n");
	Uint8 *aliceimg = (Uint8*)rd_find_data("alice_13h_64g.img");
	if ( aliceimg == NULL )
		BERP("alice_13h_64g.img not found in ramdisk");
	vga_modeset(MODE_13H);
	vga_13hsetpal(&alicepal256[0]);
	vga_13hfullblit(aliceimg,APAL256_64GRY_OFF);
}

/* mode 13h graphics demo (full image custom16) */
void demo_c16gfx( void )
{
	printk_s(SERIAL_A,"Running Mode 13h GFX demo (full image draw on custom 16 color set)\n");
	Uint8 *aliceimg = (Uint8*)rd_find_data("alice_13h_16c.img");
	if ( aliceimg == NULL )
		BERP("alice_13h_16c.img not found in ramdisk");
	Uint8 *aliceimgpal = (Uint8*)rd_find_data("alice_13h_16c.pal");
	if ( aliceimg == NULL )
		BERP("alice_13h_16c.pal not found in ramdisk");
	vga_modeset(MODE_13H);
	/* custom palette needs a bit of tweaking to adapt to 6-bit */
	int i;
	for ( i=0; i<48; i++ )
		alicepal256[i+APAL256_16USR_POS] = aliceimgpal[i]/4;
	vga_13hsetpal(&alicepal256[0]);
	vga_13hfullblit(aliceimg,APAL256_16USR_OFF);
}

/* mode 13h graphics demo (full image custom32) */
void demo_c32gfx( void )
{
	printk_s(SERIAL_A,"Running Mode 13h GFX demo (full image draw on custom 32 color set)\n");
	Uint8 *aliceimg = (Uint8*)rd_find_data("alice_13h_32c.img");
	if ( aliceimg == NULL )
		BERP("alice_13h_32c.img not found in ramdisk");
	Uint8 *aliceimgpal = (Uint8*)rd_find_data("alice_13h_32c.pal");
	if ( aliceimg == NULL )
		BERP("alice_13h_32c.pal not found in ramdisk");
	vga_modeset(MODE_13H);
	/* custom palette needs a bit of tweaking to adapt to 6-bit */
	int i;
	for ( i=0; i<96; i++ )
		alicepal256[i+APAL256_32USR_POS] = aliceimgpal[i]/4;
	vga_13hsetpal(&alicepal256[0]);
	vga_13hfullblit(aliceimg,APAL256_32USR_OFF);
}

/* mode 13h graphics demo (full image custom64) */
void demo_c64gfx( void )
{
	printk_s(SERIAL_A,"Running Mode 13h GFX demo (full image draw on custom 64 color set)\n");
	Uint8 *aliceimg = (Uint8*)rd_find_data("alice_13h_64c.img");
	if ( aliceimg == NULL )
		BERP("alice_13h_64c.img not found in ramdisk");
	Uint8 *aliceimgpal = (Uint8*)rd_find_data("alice_13h_64c.pal");
	if ( aliceimg == NULL )
		BERP("alice_13h_64c.pal not found in ramdisk");
	vga_modeset(MODE_13H);
	/* custom palette needs a bit of tweaking to adapt to 6-bit */
	int i;
	for ( i=0; i<192; i++ )
		alicepal256[i+APAL256_64USR_POS] = aliceimgpal[i]/4;
	vga_13hsetpal(&alicepal256[0]);
	vga_13hfullblit(aliceimg,APAL256_64USR_OFF);
}

/* mode 13h graphics demo (full image customfull) */
void demo_cfullgfx( void )
{
	printk_s(SERIAL_A,"Running Mode 13h GFX demo (full image draw on custom 256 color set)\n");
	Uint8 *aliceimg = (Uint8*)rd_find_data("alice_13h_fc.img");
	if ( aliceimg == NULL )
		BERP("alice_13h_fc.img not found in ramdisk");
	Uint8 *aliceimgpal = (Uint8*)rd_find_data("alice_13h_fc.pal");
	if ( aliceimg == NULL )
		BERP("alice_13h_fc.pal not found in ramdisk");
	vga_modeset(MODE_13H);
	/* custom palette needs a bit of tweaking to adapt to 6-bit */
	int i;
	for ( i=0; i<768; i++ )
		aliceimgpal[i] /= 4;
	vga_13hsetpal(aliceimgpal);
	vga_13hfullblit(aliceimg,0);
}

/* mode 13h graphics demo */
void demo_realgfx( void )
{
	printk_s(SERIAL_A,"Running Mode 13h GFX demo\n");
	vga_modeset(MODE_13H);
	vga_13hsetpal(&alicepal256[0]);
	Uint16 x,y;
	int i = 0;
	/* 16 color set */
	x = 8;
	y = 8;
	while ( i < 16 )
	{
		vga_13hdrawrect(x,y,8,8,i);
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
		vga_13hdrawrect(x,y,8,8,i);
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
		vga_13hdrawrect(x,y,8,8,i);
		x+=8;
		if ( !((++i)%8) )
		{
			x=8;
			y+=8;
		}
	}
	/* user palettes left out */
	
	/* draw some colorful noise (using the 64 color palette) */
	x = 160;
	y = 8;
	do
	{
		vga_13hputpixel(x,y,16+krand()%64);
		x++;
		if ( x >= 288 )
		{
			x = 160;
			y++;
		}
	}
	while ( (x < 288) && (y < 136) );
}
