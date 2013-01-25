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
};

/* list available demos */
void listdemos( void )
{
	unsigned int i;
	printk("Available demos:\n");
	for ( i=0; i<DEMO_COUNT; i++ )
		printk(" - %s : %s\n",demos[i].name,demos[i].desc);
}

/* 80x50 graphics demo */
void demo_blockgfx( void )
{
	printk_s(SERIAL_A,"Running Block GFX demo\n");
	vga_initimgtxt();
	vga_fullblit(&aliceimg[0]);
	vga_drawrect(50,21,25,8,4);
	/* splash screen */
	printk("%[4E%{51,11%s %s%{53,13%s.%s.%s-%s  (%s)",_kname,_kver_code,_kver_maj,_kver_min,_kver_low,_kver_suf,_karch);
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
