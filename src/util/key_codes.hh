#pragma once

#include <cstdint>

namespace spoon
{
	using key_code = uint32_t;
	
	namespace key
	{
		enum : key_code
		{
			SPACE                = 32,   /* SDLK_SPACE */
			APOSTROPHE           = 39,   /* SDLK_QUOTE */
			COMMA                = 44,   /* SDLK_COMMA */
			MINUS                = 45,   /* SDLK_MINUS */
			PERIOD               = 46,   /* SDLK_PERIOD */
			SLASH                = 47,   /* SDLK_SLASH */

			D0                   = 48,   /* SDLK_0 */
			D1                   = 49,   /* SDLK_1 */
			D2                   = 50,   /* SDLK_2 */
			D3                   = 51,   /* SDLK_3 */
			D4                   = 52,   /* SDLK_4 */
			D5                   = 53,   /* SDLK_5 */
			D6                   = 54,   /* SDLK_6 */
			D7                   = 55,   /* SDLK_7 */
			D8                   = 56,   /* SDLK_8 */
			D9                   = 57,   /* SDLK_9 */

			SEMICOLON            = 59,   /* SDLK_SEMICOLON */
			EQUAL                = 61,   /* SDLK_EQUALS */

			A                    = 97,   /* SDLK_a */
			B                    = 98,   /* SDLK_b */
			C                    = 99,   /* SDLK_c */
			D                    = 100,  /* SDLK_d */
			E                    = 101,  /* SDLK_e */
			F                    = 102,  /* SDLK_f */
			G                    = 103,  /* SDLK_g */
			H                    = 104,  /* SDLK_h */
			I                    = 105,  /* SDLK_i */
			J                    = 106,  /* SDLK_j */
			K                    = 107,  /* SDLK_k */
			L                    = 108,  /* SDLK_l */
			M                    = 109,  /* SDLK_m */
			N                    = 110,  /* SDLK_n */
			O                    = 111,  /* SDLK_o */
			P                    = 112,  /* SDLK_p */
			Q                    = 113,  /* SDLK_q */
			R                    = 114,  /* SDLK_r */
			S                    = 115,  /* SDLK_s */
			T                    = 116,  /* SDLK_t */
			U                    = 117,  /* SDLK_u */
			V                    = 118,  /* SDLK_v */
			W                    = 119,  /* SDLK_w */
			X                    = 120,  /* SDLK_x */
			Y                    = 121,  /* SDLK_y */
			Z                    = 122,  /* SDLK_z */

			LEFT_BRACKET         = 91,   /* SDLK_LEFTBRACKET */
			BACKSLASH            = 92,   /* SDLK_BACKSLASH */
			RIGHT_BRACKET        = 93,   /* SDLK_RIGHTBRACKET */
			GRAVE_ACCENT         = 96,   /* SDLK_BACKQUOTE */

			/* Function keys */
			ESCAPE               = 27,          /* SDLK_ESCAPE */
			ENTER                = 13,          /* SDLK_RETURN */
			TAB                  = 9,           /* SDLK_TAB */
			BACKSPACE            = 8,           /* SDLK_BACKSPACE */
			INSERT               = 1073741897,  /* SDLK_INSERT */
			DELETE_KEY           = 127,         /* SDLK_DELETE (переименовано во избежание конфликта с ключевым словом delete) */
			RIGHT                = 1073741903,  /* SDLK_RIGHT */
			LEFT                 = 1073741904,  /* SDLK_LEFT */
			DOWN                 = 1073741905,  /* SDLK_DOWN */
			UP                   = 1073741906,  /* SDLK_UP */
			PAGE_UP              = 1073741899,  /* SDLK_PAGEUP */
			PAGE_DOWN            = 1073741902,  /* SDLK_PAGEDOWN */
			HOME                 = 1073741898,  /* SDLK_HOME */
			END                  = 1073741901,  /* SDLK_END */
			CAPS_LOCK            = 1073741881,  /* SDLK_CAPSLOCK */
			SCROLL_LOCK          = 1073741895,  /* SDLK_SCROLLLOCK */
			NUM_LOCK             = 1073741907,  /* SDLK_NUMLOCKCLEAR */
			PRINT_SCREEN         = 1073741894,  /* SDLK_PRINTSCREEN */
			PAUSE                = 1073741896,  /* SDLK_PAUSE */
			F1                   = 1073741882,  /* SDLK_F1 */
			F2                   = 1073741883,  /* SDLK_F2 */
			F3                   = 1073741884,  /* SDLK_F3 */
			F4                   = 1073741885,  /* SDLK_F4 */
			F5                   = 1073741886,  /* SDLK_F5 */
			F6                   = 1073741887,  /* SDLK_F6 */
			F7                   = 1073741888,  /* SDLK_F7 */
			F8                   = 1073741889,  /* SDLK_F8 */
			F9                   = 1073741890,  /* SDLK_F9 */
			F10                  = 1073741891,  /* SDLK_F10 */
			F11                  = 1073741892,  /* SDLK_F11 */
			F12                  = 1073741893,  /* SDLK_F12 */
			F13                  = 1073741928,  /* SDLK_F13 */
			F14                  = 1073741929,  /* SDLK_F14 */
			F15                  = 1073741930,  /* SDLK_F15 */
			F16                  = 1073741931,  /* SDLK_F16 */
			F17                  = 1073741932,  /* SDLK_F17 */
			F18                  = 1073741933,  /* SDLK_F18 */
			F19                  = 1073741934,  /* SDLK_F19 */
			F20                  = 1073741935,  /* SDLK_F20 */
			F21                  = 1073741936,  /* SDLK_F21 */
			F22                  = 1073741937,  /* SDLK_F22 */
			F23                  = 1073741938,  /* SDLK_F23 */
			F24                  = 1073741939,  /* SDLK_F24 */

			/* Keypad */
			KP_0                 = 1073741922,  /* SDLK_KP_0 */
			KP_1                 = 1073741913,  /* SDLK_KP_1 */
			KP_2                 = 1073741914,  /* SDLK_KP_2 */
			KP_3                 = 1073741915,  /* SDLK_KP_3 */
			KP_4                 = 1073741916,  /* SDLK_KP_4 */
			KP_5                 = 1073741917,  /* SDLK_KP_5 */
			KP_6                 = 1073741918,  /* SDLK_KP_6 */
			KP_7                 = 1073741919,  /* SDLK_KP_7 */
			KP_8                 = 1073741920,  /* SDLK_KP_8 */
			KP_9                 = 1073741921,  /* SDLK_KP_9 */
			KP_DECIMAL           = 1073741923,  /* SDLK_KP_PERIOD */
			KP_DIVIDE            = 1073741908,  /* SDLK_KP_DIVIDE */
			KP_MULTIPLY          = 1073741909,  /* SDLK_KP_MULTIPLY */
			KP_SUBTRACT          = 1073741910,  /* SDLK_KP_MINUS */
			KP_ADD               = 1073741911,  /* SDLK_KP_PLUS */
			KP_ENTER             = 1073741912,  /* SDLK_KP_ENTER */
			KP_EQUAL             = 1073741927,  /* SDLK_KP_EQUALS */

			/* Modifiers */
			LEFT_SHIFT           = 1073741885,  /* SDLK_LSHIFT */
			LEFT_CONTROL         = 1073741881,  /* SDLK_LCTRL */
			LEFT_ALT             = 1073741882,  /* SDLK_LALT */
			LEFT_SUPER           = 1073741883,  /* SDLK_LGUI */
			RIGHT_SHIFT          = 1073741889,  /* SDLK_RSHIFT */
			RIGHT_CONTROL        = 1073741885,  /* SDLK_RCTRL */
			RIGHT_ALT            = 1073741886,  /* SDLK_RALT */
			RIGHT_SUPER          = 1073741887,  /* SDLK_RGUI */
			MENU                 = 1073741942   /* SDLK_MENU */
		};
	}
}
