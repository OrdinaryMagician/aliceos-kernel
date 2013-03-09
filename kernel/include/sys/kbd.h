/*
	kbd.h : PS/2 keyboard driver.
	(C)2012-2013 Marisa Kirisame, UnSX Team.
	Part of AliceOS, the Alice Operating System.
	Released under the MIT License.
*/
#ifndef KBD_H
#define KBD_H
#include <sys/types.h>
/* port I/O */
#define KBD_PORT 0x60
/* IRQ number for PS/2 keyboard */
#define KBD_IRQ 1
/* highest possible number of input handlers */
#define KBD_MAX_HANDLERS 32
/* key codes (this took A LONG TIME to write) */
#define KB_NULL      0x0000 /* NULL */
#define KB_ESC       0x0001 /* Escape */
#define KB_F1        0x0002 /* F1 */
#define KB_F2        0x0003 /* F2 */
#define KB_F3        0x0004 /* F3 */
#define KB_F4        0x0005 /* F4 */
#define KB_F5        0x0006 /* F5 */
#define KB_F6        0x0007 /* F6 */
#define KB_F7        0x0008 /* F7 */
#define KB_F8        0x0009 /* F8 */
#define KB_F9        0x000A /* F9 */
#define KB_F10       0x000B /* F10 */
#define KB_F11       0x000C /* F11 */
#define KB_F12       0x000D /* F12 */
#define KB_PRNTSCR   0x000E /* Print Screen */
#define KB_SCRLOCK   0x000F /* Scroll Lock */
#define KB_PAUSE     0x0010 /* Pause */
#define KB_TILDE     0x0011 /* ` */
#define KB_1         0x0012 /* 1 */
#define KB_2         0x0013 /* 2 */
#define KB_3         0x0014 /* 3 */
#define KB_4         0x0015 /* 4 */
#define KB_5         0x0016 /* 5 */
#define KB_6         0x0017 /* 6 */
#define KB_7         0x0018 /* 7 */
#define KB_8         0x0019 /* 8 */
#define KB_9         0x001A /* 9 */
#define KB_0         0x001B /* 0 */
#define KB_MINUS     0x001C /* - */
#define KB_PLUS      0x001D /* = */
#define KB_BSLASH    0x001E /* \ */
#define KB_BKSP      0x001F /* Backspace */
#define KB_TAB       0x0020 /* Tab */
#define KB_Q         0x0021 /* q */
#define KB_W         0x0022 /* w */
#define KB_E         0x0023 /* e */
#define KB_R         0x0024 /* r */
#define KB_T         0x0025 /* t */
#define KB_Y         0x0026 /* y */
#define KB_U         0x0027 /* u */
#define KB_I         0x0028 /* i */
#define KB_O         0x0029 /* o */
#define KB_P         0x002A /* p */
#define KB_OBRK      0x002B /* [ */
#define KB_CBRK      0x002C /* ] */
#define KB_ENTER     0x002D /* Enter (duh) */
#define KB_CAPSLCK   0x002E /* CRUISE CONTROL */
#define KB_A         0x002F /* a */
#define KB_S         0x0030 /* s */
#define KB_D         0x0031 /* d */
#define KB_F         0x0032 /* f */
#define KB_G         0x0033 /* g */
#define KB_H         0x0034 /* h */
#define KB_J         0x0035 /* j */
#define KB_K         0x0036 /* k */
#define KB_L         0x0037 /* l */
#define KB_SCOLON    0x0038 /* ; */
#define KB_APOS      0x0039 /* ' */
#define KB_LSHIFT    0x003A /* Left Shift */
#define KB_Z         0x003B /* z */
#define KB_X         0x003C /* x */
#define KB_C         0x003D /* c */
#define KB_V         0x003E /* v */
#define KB_B         0x003F /* b */
#define KB_N         0x0040 /* n */
#define KB_M         0x0041 /* m */
#define KB_COMMA     0x0042 /* , */
#define KB_PERIOD    0x0043 /* . */
#define KB_DIVIDE    0x0044 /* / */
#define KB_RSHIFT    0x0045 /* Right Shift */
#define KB_LCTRL     0x0046 /* Left Control */
#define KB_LSUPER    0x0047 /* Left Super (OS Logo) */
#define KB_LALT      0x0048 /* Left Alt (or Meta) */
#define KB_SPACE     0x0049 /* Space Bar */
#define KB_RALT      0x004A /* Right Alt (or Meta) */
#define KB_ALTGR     0x004B /* Angry alternate name for Right Alt */
#define KB_RSUPER    0x004C /* Right Super (OS Logo) */
#define KB_MENU      0x004D /* Menu */
#define KB_RCTRL     0x004E /* Right Control */
#define KB_INS       0x004F /* Insert */
#define KB_HOME      0x0050 /* Home */
#define KB_PGUP      0x0051 /* Page Up */
#define KB_DEL       0x0052 /* Delete */
#define KB_END       0x0053 /* End */
#define KB_PGDN      0x0054 /* Page Down */
#define KB_UP        0x0055 /* ↑ */
#define KB_DOWN      0x0056 /* ↓ */
#define KB_LEFT      0x0057 /* ← */
#define KB_RIGHT     0x0058 /* → */
#define KB_NUMLOCK   0x0059 /* Num Lock */
#define KB_NUMDIV    0x005A /* / */
#define KB_NUMMULT   0x005B /* * */
#define KB_NUMMINUS  0x005C /* - */
#define KB_NUMPLUS   0x005D /* + */
#define KB_NUM7      0x005E /* 7 */
#define KB_NUM8      0x005F /* 8 */
#define KB_NUM9      0x0060 /* 9 */
#define KB_NUM4      0x0061 /* 4 */
#define KB_NUM5      0x0062 /* 5 */
#define KB_NUM6      0x0063 /* 6 */
#define KB_NUM1      0x0064 /* 1 */
#define KB_NUM2      0x0065 /* 2 */
#define KB_NUM3      0x0066 /* 3 */
#define KB_NUM0      0x0067 /* 0 */
#define KB_NUMPERIOD 0x0068 /* . */
#define KB_NUMENTER  0x0069 /* Enter */
/* modifier keys */
#define KB_MOD_LCTRL  0x01
#define KB_MOD_LSHIFT 0x02
#define KB_MOD_LALT   0x04
#define KB_MOD_LSUPER 0x08
#define KB_MOD_RCTRL  0x10
#define KB_MOD_RSHIFT 0x20
#define KB_MOD_RALT   0x40
#define KB_MOD_ALTGR  0x40 /* for when the Alt key isn't angry enough */
#define KB_MOD_RSUPER 0x80
/* a key event struct */
typedef struct
{
	Uint16 code;   /* special key code (useful for non-printable keys) */
	Uint8 mod;     /* any current modifier keys applied */
	Uint8 type;    /* 0: null, 1: normal, 2: special, 3: function */
	Uint8 pressed; /* pressed or released? */
	char chr;      /* screen-printable representation */
} key_t;
/* a keyboard event handler */
typedef void (*kbd_handler_t)( key_t *key );
/* reset keyboard */
void kbd_reset( void );
/* turn on keyboard driver */
void kbd_on( void );
/* add an input handler */
Uint8 kbd_addhandler( kbd_handler_t hnd );
/* remove an input handler */
Uint8 kbd_rmhandler( kbd_handler_t hnd );
/* get a key */
void kbd_getkey( key_t *key );
#endif
