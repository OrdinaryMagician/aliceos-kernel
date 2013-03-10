/*
	kbd.c : PS/2 keyboard driver.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#include <sys/kbd.h>
#include <sys/port.h>
#include <sys/helpers.h>
#include <sys/irq_handlers.h>
#include <memops.h>
#include <printk.h>
/* internal: current mode keys */
static uint8_t kbd_cmod  = 0;
static uint8_t kbd_shift = 0; /* shift pressed? */
static uint8_t kbd_caps  = 0; /* CRUISE CONTROL ENGAGED? */
/* input handlers */
static kbd_handler_t kbd_handler[KBD_MAX_HANDLERS];
static uint8_t kbd_handlers = 0;
/* internal: qwerty layout arrays */
/* key types: 1 normal (printable), 2 special, 3 function */
static uint8_t kbd_types[128] =
{
	0,2,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,2,1,1,
	1,1,1,1,1,1,1,1,
	1,1,2,1,1,1,1,1,
	1,1,1,1,1,1,2,1,
	2,1,2,3,3,3,3,3,
	3,3,3,3,3,2,2,1,
	1,1,1,1,1,1,1,1,
	1,1,1,1,2,2,2,3,
	3,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,
};
/* when preceded by E0 */
static uint8_t kbd_typesE0[128] =
{
	0,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,
	2,2,2,2,1,2,2,2,
	2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,
	2,2,2,2,2,1,2,2,
	2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,
};
/* printable character representations */
static char kbd_chr[128] =
{
	0,'\033','1','2','3','4','5','6',
	'7','8','9','0','-','+','\b','\t',
	'q','w','e','r','t','y','u','i',
	'o','p','[',']','\n',0,'a','s',
	'd','f','g','h','j','k','l',';',
	'\'','`',0,'\\','z','x','c','v',
	'b','n','m',',','.','/',0,'*',
	0,' ',0,0,0,0,0,0,
	0,0,0,0,0,0,0,'7',
	'8','9','-','4','5','6','+','1',
	'2','3','0','.',0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
};
/* while pressing shift */
static char kbd_chrs[128] =
{
	0,0,'!','@','#','$','%','^',
	'&','*','(',')','_','=',0,0,
	'Q','W','E','R','T','Y','U','I',
	'O','P','{','}',0,0,'A','S',
	'D','F','G','H','J','K','L',':',
	'\"','~',0,'|','Z','X','C','V',
	'B','N','M','<','>','?',0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
};
/* when preceded by E0 */
static char kbd_chrE0[128] =
{
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,'\n',0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,'/',0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
};
/* numeric key codes */
static uint16_t kbd_keys[128] =
{
	KB_NULL,KB_ESC,KB_1,KB_2,KB_3,KB_4,KB_5,KB_6,
	KB_7,KB_8,KB_9,KB_0,KB_MINUS,KB_PLUS,KB_BKSP,KB_TAB,
	KB_Q,KB_W,KB_E,KB_R,KB_T,KB_Y,KB_U,KB_I,
	KB_O,KB_P,KB_OBRK,KB_CBRK,KB_ENTER,KB_LCTRL,KB_A,KB_S,
	KB_D,KB_F,KB_G,KB_H,KB_J,KB_K,KB_L,KB_SCOLON,
	KB_APOS,KB_TILDE,KB_LSHIFT,KB_BSLASH,KB_Z,KB_X,KB_C,KB_V,
	KB_B,KB_N,KB_M,KB_COMMA,KB_PERIOD,KB_DIVIDE,KB_RSHIFT,KB_NUMMULT,
	KB_LALT,KB_SPACE,KB_CAPSLCK,KB_F1,KB_F2,KB_F3,KB_F4,KB_F5,
	KB_F6,KB_F7,KB_F8,KB_F9,KB_F10,KB_NUMLOCK,KB_SCRLOCK,KB_NUM7,
	KB_NUM8,KB_NUM9,KB_NUMMINUS,KB_NUM4,KB_NUM5,KB_NUM6,KB_NUMPLUS,KB_NUM1,
	KB_NUM2,KB_NUM3,KB_NUM0,KB_NUMPERIOD,0xFF54,0xFF55,0xFF56,KB_F11,
	KB_F12,0xFF59,0xFF5A,0xFF5B,0xFF5C,0xFF5D,0xFF5E,0xFF5F,
	0xFF60,0xFF61,0xFF62,0xFF63,0xFF64,0xFF65,0xFF66,0xFF67,
	0xFF68,0xFF69,0xFF6A,0xFF6B,0xFF6C,0xFF6D,0xFF6E,0xFF6F,
	0xFF70,0xFF71,0xFF72,0xFF73,0xFF74,0xFF75,0xFF76,0xFF77,
	0xFF78,0xFF79,0xFF7A,0xFF7B,0xFF7C,0xFF7D,0xFF7E,0xFF7F,
};
/* when preceded by 0xE0 */
static uint16_t kbd_keysE0[128] =
{
	KB_NULL,0xFF81,0xFF82,0xFF83,0xFF84,0xFF85,0xFF86,0xFF87,
	0xFF88,0xFF89,0xFF8A,0xFF8B,0xFF8C,0xFF8D,0xFF8E,0xFF8F,
	0xFF90,0xFF91,0xFF92,0xFF93,0xFF94,0xFF95,0xFF96,0xFF97,
	0xFF98,0xFF99,0xFF9A,0xFF9B,KB_NUMENTER,KB_RCTRL,0xFF9E,0xFF9F,
	0xFFA0,0xFFA1,0xFFA2,0xFFA3,0xFFA4,0xFFA5,0xFFA6,0xFFA7,
	0xFFA8,0xFFA9,0xFFAA,0xFFAB,0xFFAC,0xFFAD,0xFFAE,0xFFAF,
	0xFFB0,0xFFB1,0xFFB2,0xFFB3,0xFFB4,KB_NUMDIV,0xFFB6,0xFFB7,
	KB_RALT,0xFFB9,0xFFBA,0xFFBB,0xFFBC,0xFFBD,0xFFBE,KB_PRNTSCR,
	0xFFC0,0xFFC1,0xFFC2,0xFFC3,0xFFC4,0xFFC5,0xFFC6,KB_HOME,
	KB_UP,KB_PGUP,0xFFCA,KB_LEFT,0xFFCC,KB_RIGHT,0xFFCE,KB_END,
	KB_DOWN,KB_PGDN,KB_INS,KB_DEL,0xFFD4,0xFFD5,0xFFD6,0xFFD7,
	0xFFD8,0xFFD9,0xFFDA,KB_LSUPER,KB_RSUPER,KB_MENU,0xFFDE,0xFFDF,
	0xFFE0,0xFFE1,0xFFE2,0xFFE3,0xFFE4,0xFFE5,0xFFE6,0xFFE7,
	0xFFE8,0xFFE9,0xFFEA,0xFFEB,0xFFEC,0xFFED,0xFFEE,0xFFEF,
	0xFFF0,0xFFF1,0xFFF2,0xFFF3,0xFFF4,0xFFF5,0xFFF6,0xFFF7,
	0xFFF8,0xFFF9,0xFFFA,0xFFFB,0xFFFC,0xFFFD,0xFFFE,0xFFFF,
};
/* parse a raw key into a key_t format */
static void kbd_parsekey( char *key, key_t *pkey )
{
	uint8_t i = 0;
	if ( key[0] == 0xE1 ) /* damn pause key why are you so long */
	{
		pkey->type = 2;
		pkey->code = KB_PAUSE;
		pkey->mod = kbd_cmod;
		return;
	}
	if ( key[0] == 0xE0 ) /* some extra keys are handled differently */
		i++;
	if ( i )
	{
		pkey->type = kbd_typesE0[key[i]&0x7F];
		pkey->code = kbd_keysE0[key[i]&0x7F];
	}
	else
	{
		pkey->type = kbd_types[key[i]&0x7F];
		pkey->code = kbd_keys[key[i]&0x7F];
	}
	pkey->pressed = !((key[i]>>7)&1);
	if ( pkey->code == KB_LCTRL )
	{
		kbd_cmod &= ~KB_MOD_LCTRL;
		kbd_cmod |= KB_MOD_LCTRL*pkey->pressed;
	}
	if ( pkey->code == KB_LSHIFT )
	{
		kbd_cmod &= ~KB_MOD_LSHIFT;
		kbd_cmod |= KB_MOD_LSHIFT*pkey->pressed;
		kbd_shift = pkey->pressed;
	}
	if ( pkey->code == KB_LALT )
	{
		kbd_cmod &= ~KB_MOD_LALT;
		kbd_cmod |= KB_MOD_LALT*pkey->pressed;
	}
	if ( pkey->code == KB_LSUPER )
	{
		kbd_cmod &= ~KB_MOD_LSUPER;
		kbd_cmod |= KB_MOD_LSUPER*pkey->pressed;
	}
	if ( pkey->code == KB_RCTRL )
	{
		kbd_cmod &= ~KB_MOD_RCTRL;
		kbd_cmod |= KB_MOD_RCTRL*pkey->pressed;
	}
	if ( pkey->code == KB_RSHIFT )
	{
		kbd_cmod &= ~KB_MOD_RSHIFT;
		kbd_cmod |= KB_MOD_RSHIFT*pkey->pressed;
		kbd_shift = pkey->pressed;
	}
	if ( pkey->code == KB_RALT )
	{
		kbd_cmod &= ~KB_MOD_RALT;
		kbd_cmod |= KB_MOD_RALT*pkey->pressed;
	}
	if ( pkey->code == KB_RSUPER )
	{
		kbd_cmod &= ~KB_MOD_RSUPER;
		kbd_cmod |= KB_MOD_RSUPER*pkey->pressed;
	}
	if ( pkey->code == KB_CAPSLCK )
	{
		if ( pkey->pressed )
			kbd_caps = !kbd_caps;
	}
	pkey->mod = kbd_cmod;
	if ( i )
		pkey->chr = kbd_chrE0[key[i]&0x7F];
	else if ( kbd_shift && kbd_chrs[key[i]&0x7F] )
		pkey->chr = kbd_chrs[key[i]&0x7F];
	else
		pkey->chr = kbd_chr[key[i]&0x7F];
	if ( kbd_caps && (pkey->chr>=0x41) && (pkey->chr<=0x5A) )
		pkey->chr += 0x20;
	else if ( kbd_caps && (pkey->chr>=0x61) && (pkey->chr<=0x7A) )
		pkey->chr -= 0x20;
}
static char kbd_getch( void )
{
	while (inport_b(KBD_PORT+4)==0x1C);
	return inport_b(KBD_PORT);
}
/* get a key */
void kbd_getkey( key_t *key )
{
	if ( !key )
		return;
	char got[6] = {0,0,0,0,0,0};
	got[0] = kbd_getch();
	if ( got[0] == 0xE0 )
		got[1] = kbd_getch();
	else if ( got[0] == 0xE1 )
	{
		got[1] = kbd_getch();
		got[2] = kbd_getch();
		got[3] = kbd_getch();
		got[4] = kbd_getch();
		got[5] = kbd_getch();
	}
	memset((uint8_t*)key,0,sizeof(key_t));
	kbd_parsekey(got,key);
}
static void kbd_basehandler( regs_t *regs )
{
	int_enable();
	key_t parsed;
	kbd_getkey(&parsed);
	uint8_t i;
	for ( i=0; i<kbd_handlers; i++ )
	{
		kbd_handler_t hnd = kbd_handler[i];
		if ( hnd )
			hnd(&parsed);
	}
	irq_eoi(KBD_IRQ);
}
/* turn on keyboard driver */
void kbd_on( void )
{
	memset((uint8_t*)&kbd_handler[0],0,
		sizeof(kbd_handler_t)*KBD_MAX_HANDLERS);
	kbd_handlers = 0;
	register_irq_handler(KBD_IRQ,kbd_basehandler);
}
/* reset keyboard */
void kbd_reset( void )
{
	uint8_t tmp = inport_b(KBD_PORT+1);
	outport_b(KBD_PORT+1,tmp|0x80);
	outport_b(KBD_PORT+1,tmp&0x7F);
	inport_b(KBD_PORT+1);
}
/* add an input handler */
uint8_t kbd_addhandler( kbd_handler_t hnd )
{
	if ( kbd_handlers == KBD_MAX_HANDLERS ) /* list full */
		return 1;
	kbd_handler[kbd_handlers] = hnd;
	kbd_handlers++;
	return 0;
}
static void kbd_rmhandlerid( uint8_t id )
{
	memmove((uint8_t*)&kbd_handler[id],(uint8_t*)&kbd_handler[id+1],
		sizeof(kbd_handler_t)*(kbd_handlers-id));
	kbd_handlers--;
}
/* remove an input handler */
uint8_t kbd_rmhandler( kbd_handler_t hnd )
{
	uint8_t i;
	for ( i=0; i<kbd_handlers; i++ )
	{
		if ( kbd_handler[i] != hnd )
			continue;
		kbd_rmhandlerid(i);
		return 0;
	}
	return 1;
}
