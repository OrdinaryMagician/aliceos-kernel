/*
	vgafont.c : BIOS font patches.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/types.h>
#include <vga/vgareg.h>
#include <vga/vgafont.h>
#include <memops.h>
/* change a specific glyph in the character map */
void setglyph( uint8_t val, uint8_t *data )
{
	/* need to change some stuff, like plane to write to and stuff */
	setvgareg(VGA_SEQ,VGA_SEQ_MMASK,0x04);
	setvgareg(VGA_SEQ,VGA_SEQ_CMSEL,0x00);
	uint8_t old_smmod = getvgareg(VGA_SEQ,VGA_SEQ_SMMOD);
	setvgareg(VGA_SEQ,VGA_SEQ_SMMOD,0x06);
	setvgareg(VGA_GC,VGA_GC_RDMSEL,0x02);
	uint8_t old_gfxmod = getvgareg(VGA_GC,VGA_GC_GFXMOD);
	setvgareg(VGA_GC,VGA_GC_GFXMOD,0x00);
	int i;
	uint8_t *fntmem = (uint8_t*)0xB8000;
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
void getglyph( uint8_t val, uint8_t *data )
{
	/* need to change some stuff, like plane to write to and stuff */
	setvgareg(VGA_SEQ,VGA_SEQ_MMASK,0x04);
	setvgareg(VGA_SEQ,VGA_SEQ_CMSEL,0x00);
	uint8_t old_smmod = getvgareg(VGA_SEQ,VGA_SEQ_SMMOD);
	setvgareg(VGA_SEQ,VGA_SEQ_SMMOD,0x06);
	setvgareg(VGA_GC,VGA_GC_RDMSEL,0x02);
	uint8_t old_gfxmod = getvgareg(VGA_GC,VGA_GC_GFXMOD);
	setvgareg(VGA_GC,VGA_GC_GFXMOD,0x00);
	int i;
	uint8_t *fntmem = (uint8_t*)0xB8000;
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
/* get the current font */
void getfont( uint8_t *font )
{
	/* need to change some stuff, like plane to write to and stuff */
	setvgareg(VGA_SEQ,VGA_SEQ_MMASK,0x04);
	setvgareg(VGA_SEQ,VGA_SEQ_CMSEL,0x00);
	uint8_t old_smmod = getvgareg(VGA_SEQ,VGA_SEQ_SMMOD);
	setvgareg(VGA_SEQ,VGA_SEQ_SMMOD,0x06);
	setvgareg(VGA_GC,VGA_GC_RDMSEL,0x02);
	uint8_t old_gfxmod = getvgareg(VGA_GC,VGA_GC_GFXMOD);
	setvgareg(VGA_GC,VGA_GC_GFXMOD,0x00);
	int i, j;
	/* copy character map, skip 16 bytes of padding for each glyph */
	uint8_t *fntmem = (uint8_t*)0xB8000;
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
/* apply patches to the BIOS font, save a local copy */
void initfont( void )
{
	uint8_t chr[16];
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
/* change the system font using a FBFONT formatted font as reference */
void setfont_256( uint8_t *font )
{
	uint8_t chr[16];
	int i, j;
	uint8_t *fch;
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
void getfont_256( uint8_t *font )
{
	uint8_t chr[16];
	int i, j;
	uint8_t *fch;
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
void setfont( uint8_t *font )
{
	/* need to change some stuff, like plane to write to and stuff */
	setvgareg(VGA_SEQ,VGA_SEQ_MMASK,0x04);
	setvgareg(VGA_SEQ,VGA_SEQ_CMSEL,0x00);
	uint8_t old_smmod = getvgareg(VGA_SEQ,VGA_SEQ_SMMOD);
	setvgareg(VGA_SEQ,VGA_SEQ_SMMOD,0x06);
	setvgareg(VGA_GC,VGA_GC_RDMSEL,0x02);
	uint8_t old_gfxmod = getvgareg(VGA_GC,VGA_GC_GFXMOD);
	setvgareg(VGA_GC,VGA_GC_GFXMOD,0x00);
	int i, j;
	/* copy character map, skip 16 bytes of padding for each glyph */
	uint8_t *fntmem = (uint8_t*)0xB8000;
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
