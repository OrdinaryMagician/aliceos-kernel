/*
	font.c : BIOS font patches.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <vga/vgafont.h>
#include <vga/vgareg.h>
#include <memops.h>

/* apply patches to the BIOS font, save a local copy */
void initfont( void )
{
	Uint8 chr[16];
	/* correct misproportioned half block characters 0xDF and 0xDC */
	getglyph(0xDF,&chr[0]);
	chr[7] = 0xFF;
	setglyph(0xDF,&chr[0]);
	getglyph(0xDC,&chr[0]);
	chr[7] = 0x00;
	setglyph(0xDC,&chr[0]);
	/* save font */
	getfont(&biosfnt[0]);
}

/* change the font using a byte-per-pixel font as reference instead of a bit-per-pixel font */
void setfont_256( Uint8 *font )
{
	Uint8 chr[16];
	int i, j;
	Uint8 *fch;
	for ( i=0; i<256; i++ )
	{
		fch = font+(i*128);
		for ( j=0; j<16; j++ )
		{
			chr[j] |= (fch[0]&1)<<7;
			chr[j] |= (fch[1]&1)<<6;
			chr[j] |= (fch[2]&1)<<5;
			chr[j] |= (fch[3]&1)<<4;
			chr[j] |= (fch[4]&1)<<3;
			chr[j] |= (fch[5]&1)<<2;
			chr[j] |= (fch[6]&1)<<1;
			chr[j] |= (fch[7]&1);
		}
		setglyph(i,&chr[0]);
		memset(&chr[0],0,16);
	}
}

/* get the current font converting from the BIOS format to FBFONT format */
void getfont_256( Uint8 *font )
{
	Uint8 chr[16];
	int i, j;
	Uint8 *fch;
	for ( i=0; i<256; i++ )
	{
		getglyph(i,&chr[0]);
		fch = font+(i*128);
		for ( j=0; j<16; j++ )
		{
			fch[0] = ((chr[j]>>7)&1)*255;
			fch[1] = ((chr[j]>>6)&1)*255;
			fch[2] = ((chr[j]>>5)&1)*255;
			fch[3] = ((chr[j]>>4)&1)*255;
			fch[4] = ((chr[j]>>3)&1)*255;
			fch[5] = ((chr[j]>>2)&1)*255;
			fch[6] = ((chr[j]>>1)&1)*255;
			fch[7] = ((chr[j])&1)*255;
		}
	}
}

/* change the font */
void setfont( Uint8 *font )
{
	/* need to change some stuff, like plane to write to and stuff */
	setvgareg(VGA_SEQ,VGA_SEQ_MMASK,0x04);
	setvgareg(VGA_SEQ,VGA_SEQ_CMSEL,0x00);
	Uint8 old_smmod = getvgareg(VGA_SEQ,VGA_SEQ_SMMOD);
	setvgareg(VGA_SEQ,VGA_SEQ_SMMOD,0x06);
	setvgareg(VGA_GC,VGA_GC_RDMSEL,0x02);
	Uint8 old_gfxmod = getvgareg(VGA_GC,VGA_GC_GFXMOD);
	setvgareg(VGA_GC,VGA_GC_GFXMOD,0x00);
	int i, j;
	/* copy character map, skip 16 bytes of padding for each glyph */
	Uint8 *fntmem = (Uint8*)0xB8000;
	for ( i=0; i<256; i++ )
	{
		for ( j=0; j<16; j++ )
			*(fntmem++) = *(font++);
		fntmem += 16;
	}
	/* restore to defaults */
	setvgareg(VGA_SEQ,VGA_SEQ_MMASK,0x03);
	setvgareg(VGA_SEQ,VGA_SEQ_SMMOD,old_smmod);
	setvgareg(VGA_GC,VGA_GC_RDMSEL,0x00);
	setvgareg(VGA_GC,VGA_GC_GFXMOD,old_gfxmod);
}

/* get the current font */
void getfont( Uint8 *font )
{
	/* need to change some stuff, like plane to write to and stuff */
	setvgareg(VGA_SEQ,VGA_SEQ_MMASK,0x04);
	setvgareg(VGA_SEQ,VGA_SEQ_CMSEL,0x00);
	Uint8 old_smmod = getvgareg(VGA_SEQ,VGA_SEQ_SMMOD);
	setvgareg(VGA_SEQ,VGA_SEQ_SMMOD,0x06);
	setvgareg(VGA_GC,VGA_GC_RDMSEL,0x02);
	Uint8 old_gfxmod = getvgareg(VGA_GC,VGA_GC_GFXMOD);
	setvgareg(VGA_GC,VGA_GC_GFXMOD,0x00);
	int i, j;
	/* copy character map, skip 16 bytes of padding for each glyph */
	Uint8 *fntmem = (Uint8*)0xB8000;
	for ( i=0; i<256; i++ )
	{
		for ( j=0; j<16; j++ )
			*(font++) = *(fntmem++);
		fntmem += 16;
	}
	/* restore to defaults */
	setvgareg(VGA_SEQ,VGA_SEQ_MMASK,0x03);
	setvgareg(VGA_SEQ,VGA_SEQ_SMMOD,old_smmod);
	setvgareg(VGA_GC,VGA_GC_RDMSEL,0x00);
	setvgareg(VGA_GC,VGA_GC_GFXMOD,old_gfxmod);
}

/* change a specific glyph in the character map */
void setglyph( Uint8 val, Uint8 *data )
{
	/* need to change some stuff, like plane to write to and stuff */
	setvgareg(VGA_SEQ,VGA_SEQ_MMASK,0x04);
	setvgareg(VGA_SEQ,VGA_SEQ_CMSEL,0x00);
	Uint8 old_smmod = getvgareg(VGA_SEQ,VGA_SEQ_SMMOD);
	setvgareg(VGA_SEQ,VGA_SEQ_SMMOD,0x06);
	setvgareg(VGA_GC,VGA_GC_RDMSEL,0x02);
	Uint8 old_gfxmod = getvgareg(VGA_GC,VGA_GC_GFXMOD);
	setvgareg(VGA_GC,VGA_GC_GFXMOD,0x00);
	int i;
	Uint8 *fntmem = (Uint8*)0xB8000;
	/* skip characters */
	for ( i=0; i<val; i++ )
		fntmem += 32;
	for ( i=0; i<16; i++ )
		*(fntmem++) = *(data++);
	/* restore to defaults */
	setvgareg(VGA_SEQ,VGA_SEQ_MMASK,0x03);
	setvgareg(VGA_SEQ,VGA_SEQ_SMMOD,old_smmod);
	setvgareg(VGA_GC,VGA_GC_RDMSEL,0x00);
	setvgareg(VGA_GC,VGA_GC_GFXMOD,old_gfxmod);
}

/* retrieve a specific glyph in the character map */
void getglyph( Uint8 val, Uint8 *data )
{
	/* need to change some stuff, like plane to write to and stuff */
	setvgareg(VGA_SEQ,VGA_SEQ_MMASK,0x04);
	setvgareg(VGA_SEQ,VGA_SEQ_CMSEL,0x00);
	Uint8 old_smmod = getvgareg(VGA_SEQ,VGA_SEQ_SMMOD);
	setvgareg(VGA_SEQ,VGA_SEQ_SMMOD,0x06);
	setvgareg(VGA_GC,VGA_GC_RDMSEL,0x02);
	Uint8 old_gfxmod = getvgareg(VGA_GC,VGA_GC_GFXMOD);
	setvgareg(VGA_GC,VGA_GC_GFXMOD,0x00);
	int i;
	Uint8 *fntmem = (Uint8*)0xB8000;
	/* skip characters */
	for ( i=0; i<val; i++ )
		fntmem += 32;
	for ( i=0; i<16; i++ )
		*(data++) = *(fntmem++);
	/* restore to defaults */
	setvgareg(VGA_SEQ,VGA_SEQ_MMASK,0x03);
	setvgareg(VGA_SEQ,VGA_SEQ_SMMOD,old_smmod);
	setvgareg(VGA_GC,VGA_GC_RDMSEL,0x00);
	setvgareg(VGA_GC,VGA_GC_GFXMOD,old_gfxmod);
}
