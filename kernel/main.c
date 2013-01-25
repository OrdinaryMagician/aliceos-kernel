/*
	kmain.c : C entry point for the AliceOS kernel.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <types.h>
#include <helpers.h>
#include <kdefs.h>
#include <vgatext.h>
#include <multiboot.h>
#include <krand.h>
#include <printk.h>
#include <serial.h>
#include <cmos.h>
#include <berp.h>
#include <palettes.h>
#include <vgaimgtxt.h>
#include <vgafont.h>
#include <vgamodeset.h>
#include <aliceimg.h>
#include <vga13hgfx.h>

Uint32 *initial_esp;

/* 80x50 graphics demo */
void demo_blockgfx( void )
{
	vga_initimgtxt();
	vga_fullblit(&aliceimg[0]);
	vga_drawrect(50,21,25,8,4);
	/* splash screen */
	printk("%[4E%{51,11%s %s%{53,13%s.%s.%s-%s  (%s)",_kname,_kver_code,_kver_maj,_kver_min,_kver_low,_kver_suf,_karch);
}

/* mode 13h graphics demo */
void demo_realgfx( void )
{
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

/* serial output */
void init_serial( void )
{
	serial_ins();
}

void init_console( void )
{
	/* initialize some vga settings */
	vga_initfont();
	vga_modeset(MODE_3H);
	vga_setpal(&alicepal[0]);
	vga_set8dot();

	/* pretty screen fill */
	vga_clr_s();
	vga_showcursor();
	vga_curset(0,0);
	vga_setcroparea(2,25);	/* only things below the header should scroll */
}

void print_header( void )
{
	vga_setattr(APAL_CYAN,APAL_BLUE);
	vga_putmc(' ',80);
	printk("%{1,0%s %s - %s.%s.%s-%s (%s)",_kname,_kver_code,_kver_maj,_kver_min,_kver_low,_kver_suf,_karch);
	printk_s(SERIAL_A,"\033[1;36m%s %s - %s.%s.%s-%s (%s)\033[0m\n",_kname,_kver_code,_kver_maj,_kver_min,_kver_low,_kver_suf,_karch);
	vga_curset(0,2);
	vga_setattr(APAL_WHITE,APAL_BLACK);

	/* pretty print time and date */
	Uint8 cmosval[128];
	cmos_dump(&cmosval[0]);
	char weekdays[7][4] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	vga_setattr(APAL_CYAN,APAL_BLUE);
	printk("%{-26,0%s %02x/%02x/20%02x | %02x:%02x:%02x",weekdays[clamp(cmosval[6],1,7)-1],cmosval[7],cmosval[8],cmosval[9],cmosval[4],cmosval[2],cmosval[0]);
	vga_curset(0,2);
	vga_setattr(APAL_WHITE,APAL_BLACK);
}

/* C entry point for the kernel starts here. */
int kmain( struct multiboot *mboot, Uint32 mboot_mag, Uint32 *esp )
{
	/* set the initial stack pointer */
	initial_esp = esp;

	/* start stuff */
	init_serial();
	init_console();
	print_header();
	
	/* insert demo code or whatever here */
	demo_realgfx();

	/* THE END */
	printk_s(SERIAL_A,"All done!\n");
	return 0xADEADBED;
}
