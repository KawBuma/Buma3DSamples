#pragma once
#include <stdint.h>

namespace buma
{
namespace input
{

struct INPUT_PRESS_PARAMS
{
	uint64_t press_count;	// frame
	float press_time;		// ms
	bool is_release;
};

#pragma region Mouse

struct INPUT_POS_PARAMS
{
	int val;
	int old;
	int delta;
};

struct MOUSE_PRESS_DATA
{
	INPUT_PRESS_PARAMS left;
	INPUT_PRESS_PARAMS middle;
	INPUT_PRESS_PARAMS right;
	INPUT_PRESS_PARAMS x_button1;
	INPUT_PRESS_PARAMS x_button2;
};

struct MOUSE_DATA
{
	MOUSE_PRESS_DATA buttons;
	INPUT_POS_PARAMS x;
	INPUT_POS_PARAMS y;
	INPUT_POS_PARAMS rot;
};

#pragma endregion Mouse

#pragma region Keyboard

enum eKeys
{
	  Back = 0
	, eKeys_Tab

    , eKeys_Enter

    , eKeys_Pause
    , eKeys_CapsLock
    , eKeys_Kana

    , eKeys_Kanji

    , eKeys_Escape
    , eKeys_ImeConvert
    , eKeys_ImeNoConvert

    , eKeys_Space
    , eKeys_PageUp
    , eKeys_PageDown
    , eKeys_End
    , eKeys_Home
    , eKeys_Left
    , eKeys_Up
    , eKeys_Right
    , eKeys_Down
    , eKeys_Select
    , eKeys_Print
    , eKeys_Execute
    , eKeys_PrintScreen
    , eKeys_Insert
    , eKeys_Delete
    , eKeys_Help
    , eKeys_D0
    , eKeys_D1
    , eKeys_D2
    , eKeys_D3
    , eKeys_D4
    , eKeys_D5
    , eKeys_D6
    , eKeys_D7
    , eKeys_D8
    , eKeys_D9

    , eKeys_A
    , eKeys_B
    , eKeys_C
    , eKeys_D
    , eKeys_E
    , eKeys_F
    , eKeys_G
    , eKeys_H
    , eKeys_I
    , eKeys_J
    , eKeys_K
    , eKeys_L
    , eKeys_M
    , eKeys_N
    , eKeys_O
    , eKeys_P
    , eKeys_Q
    , eKeys_R
    , eKeys_S
    , eKeys_T
    , eKeys_U
    , eKeys_V
    , eKeys_W
    , eKeys_X
    , eKeys_Y
    , eKeys_Z
    , eKeys_LeftWindows
    , eKeys_RightWindows
    , eKeys_Apps

    , eKeys_Sleep
    , eKeys_NumPad0
    , eKeys_NumPad1
    , eKeys_NumPad2
    , eKeys_NumPad3
    , eKeys_NumPad4
    , eKeys_NumPad5
    , eKeys_NumPad6
    , eKeys_NumPad7
    , eKeys_NumPad8
    , eKeys_NumPad9
    , eKeys_Multiply
    , eKeys_Add
    , eKeys_Separator
    , eKeys_Subtract

    , eKeys_Decimal
    , eKeys_Divide
    , eKeys_F1
    , eKeys_F2
    , eKeys_F3
    , eKeys_F4
    , eKeys_F5
    , eKeys_F6
    , eKeys_F7
    , eKeys_F8
    , eKeys_F9
    , eKeys_F10
    , eKeys_F11
    , eKeys_F12
    , eKeys_F13
    , eKeys_F14
    , eKeys_F15
    , eKeys_F16
    , eKeys_F17
    , eKeys_F18
    , eKeys_F19
    , eKeys_F20
    , eKeys_F21
    , eKeys_F22
    , eKeys_F23
    , eKeys_F24

    , eKeys_NumLock
    , eKeys_Scroll

    , eKeys_LeftShift
    , eKeys_RightShift
    , eKeys_LeftControl
    , eKeys_RightControl
    , eKeys_LeftAlt
    , eKeys_RightAlt
    , eKeys_BrowserBack
    , eKeys_BrowserForward
    , eKeys_BrowserRefresh
    , eKeys_BrowserStop
    , eKeys_BrowserSearch
    , eKeys_BrowserFavorites
    , eKeys_BrowserHome
    , eKeys_VolumeMute
    , eKeys_VolumeDown
    , eKeys_VolumeUp
    , eKeys_MediaNextTrack
    , eKeys_MediaPreviousTrack
    , eKeys_MediaStop
    , eKeys_MediaPlayPause
    , eKeys_LaunchMail
    , eKeys_SelectMedia
    , eKeys_LaunchApplication1
    , eKeys_LaunchApplication2

    , eKeys_OemSemicolon
    , eKeys_OemPlus
    , eKeys_OemComma
    , eKeys_OemMinus
    , eKeys_OemPeriod
    , eKeys_OemQuestion
    , eKeys_OemTilde

    , eKeys_OemOpenBrackets
    , eKeys_OemPipe
    , eKeys_OemCloseBrackets
    , eKeys_OemQuotes
    , eKeys_Oem8

    , eKeys_OemBackslash

    , eKeys_ProcessKey

    , eKeys_OemCopy
    , eKeys_OemAuto
    , eKeys_OemEnlW

    , eKeys_Attn
    , eKeys_Crsel
    , eKeys_Exsel
    , eKeys_EraseEof
    , eKeys_Play
    , eKeys_Zoom

    , eKeys_Pa1
    , eKeys_OemClear

	, eKeys_NumKeys
};
union KEY_PRESS_DATA
{
	INPUT_PRESS_PARAMS press_params[eKeys::eKeys_NumKeys];
	struct {
	INPUT_PRESS_PARAMS Back					 ; // VK_BACK					, 0x8
	INPUT_PRESS_PARAMS Tab					 ; // VK_TAB					, 0x9
	INPUT_PRESS_PARAMS Enter				 ; // VK_RETURN					, 0xD
	INPUT_PRESS_PARAMS Pause				 ; // VK_PAUSE					, 0x13
	INPUT_PRESS_PARAMS CapsLock				 ; // VK_CAPITAL				, 0x14
	INPUT_PRESS_PARAMS Kana					 ; // VK_KANA					, 0x15
	INPUT_PRESS_PARAMS Kanji				 ; // VK_KANJI					, 0x19
	INPUT_PRESS_PARAMS Escape				 ; // VK_ESCAPE					, 0x1B
	INPUT_PRESS_PARAMS ImeConvert			 ; // VK_CONVERT				, 0x1C
	INPUT_PRESS_PARAMS ImeNoConvert			 ; // VK_NONCONVERT				, 0x1D
	INPUT_PRESS_PARAMS Space				 ; // VK_SPACE					, 0x20
	INPUT_PRESS_PARAMS PageUp				 ; // VK_PRIOR					, 0x21
	INPUT_PRESS_PARAMS PageDown				 ; // VK_NEXT					, 0x22
	INPUT_PRESS_PARAMS End					 ; // VK_END					, 0x23
	INPUT_PRESS_PARAMS Home					 ; // VK_HOME					, 0x24
	INPUT_PRESS_PARAMS Left					 ; // VK_LEFT					, 0x25
	INPUT_PRESS_PARAMS Up					 ; // VK_UP						, 0x26
	INPUT_PRESS_PARAMS Right				 ; // VK_RIGHT					, 0x27
	INPUT_PRESS_PARAMS Down					 ; // VK_DOWN					, 0x28
	INPUT_PRESS_PARAMS Select				 ; // VK_SELECT					, 0x29
	INPUT_PRESS_PARAMS Print				 ; // VK_PRINT					, 0x2A
	INPUT_PRESS_PARAMS Execute				 ; // VK_EXECUTE				, 0x2B
	INPUT_PRESS_PARAMS PrintScreen			 ; // VK_SNAPSHOT				, 0x2C
	INPUT_PRESS_PARAMS Insert				 ; // VK_INSERT					, 0x2D
	INPUT_PRESS_PARAMS Delete				 ; // VK_DELETE					, 0x2E
	INPUT_PRESS_PARAMS Help					 ; // VK_HELP					, 0x2F
	INPUT_PRESS_PARAMS D0					 ; //							  0x30
	INPUT_PRESS_PARAMS D1					 ; //							  0x31
	INPUT_PRESS_PARAMS D2					 ; //							  0x32
	INPUT_PRESS_PARAMS D3					 ; //							  0x33
	INPUT_PRESS_PARAMS D4					 ; //							  0x34
	INPUT_PRESS_PARAMS D5					 ; //							  0x35
	INPUT_PRESS_PARAMS D6					 ; //							  0x36
	INPUT_PRESS_PARAMS D7					 ; //							  0x37
	INPUT_PRESS_PARAMS D8					 ; //							  0x38
	INPUT_PRESS_PARAMS D9					 ; //							  0x39
	INPUT_PRESS_PARAMS A					 ; //							  0x41
	INPUT_PRESS_PARAMS B					 ; //							  0x42
	INPUT_PRESS_PARAMS C					 ; //							  0x43
	INPUT_PRESS_PARAMS D					 ; //							  0x44
	INPUT_PRESS_PARAMS E					 ; //							  0x45
	INPUT_PRESS_PARAMS F					 ; //							  0x46
	INPUT_PRESS_PARAMS G					 ; //							  0x47
	INPUT_PRESS_PARAMS H					 ; //							  0x48
	INPUT_PRESS_PARAMS I					 ; //							  0x49
	INPUT_PRESS_PARAMS J					 ; //							  0x4A
	INPUT_PRESS_PARAMS K					 ; //							  0x4B
	INPUT_PRESS_PARAMS L					 ; //							  0x4C
	INPUT_PRESS_PARAMS M					 ; //							  0x4D
	INPUT_PRESS_PARAMS N					 ; //							  0x4E
	INPUT_PRESS_PARAMS O					 ; //							  0x4F
	INPUT_PRESS_PARAMS P					 ; //							  0x50
	INPUT_PRESS_PARAMS Q					 ; //							  0x51
	INPUT_PRESS_PARAMS R					 ; //							  0x52
	INPUT_PRESS_PARAMS S					 ; //							  0x53
	INPUT_PRESS_PARAMS T					 ; //							  0x54
	INPUT_PRESS_PARAMS U					 ; //							  0x55
	INPUT_PRESS_PARAMS V					 ; //							  0x56
	INPUT_PRESS_PARAMS W						 ; //							  0x57
	INPUT_PRESS_PARAMS X					 ; //							  0x58
	INPUT_PRESS_PARAMS Y					 ; //							  0x59
	INPUT_PRESS_PARAMS Z					 ; //							  0x5A
	INPUT_PRESS_PARAMS LeftWindows			 ; // VK_LWIN					, 0x5B
	INPUT_PRESS_PARAMS RightWindows			 ; // VK_RWIN					, 0x5C
	INPUT_PRESS_PARAMS Apps					 ; // VK_APPS					, 0x5D
	INPUT_PRESS_PARAMS Sleep				 ; // VK_SLEEP					, 0x5F
	INPUT_PRESS_PARAMS NumPad0				 ; // VK_NUMPAD0				, 0x60
	INPUT_PRESS_PARAMS NumPad1				 ; // VK_NUMPAD1				, 0x61
	INPUT_PRESS_PARAMS NumPad2				 ; // VK_NUMPAD2				, 0x62
	INPUT_PRESS_PARAMS NumPad3				 ; // VK_NUMPAD3				, 0x63
	INPUT_PRESS_PARAMS NumPad4				 ; // VK_NUMPAD4				, 0x64
	INPUT_PRESS_PARAMS NumPad5				 ; // VK_NUMPAD5				, 0x65
	INPUT_PRESS_PARAMS NumPad6				 ; // VK_NUMPAD6				, 0x66
	INPUT_PRESS_PARAMS NumPad7				 ; // VK_NUMPAD7				, 0x67
	INPUT_PRESS_PARAMS NumPad8				 ; // VK_NUMPAD8				, 0x68
	INPUT_PRESS_PARAMS NumPad9				 ; // VK_NUMPAD9				, 0x69
	INPUT_PRESS_PARAMS Multiply				 ; // VK_MULTIPLY				, 0x6A
	INPUT_PRESS_PARAMS Add					 ; // VK_ADD					, 0x6B
	INPUT_PRESS_PARAMS Separator			 ; // VK_SEPARATOR				, 0x6C
	INPUT_PRESS_PARAMS Subtract				 ; // VK_SUBTRACT				, 0x6D
	INPUT_PRESS_PARAMS Decimal				 ; // VK_DECIMANL				, 0x6E
	INPUT_PRESS_PARAMS Divide				 ; // VK_DIVIDE					, 0x6F
	INPUT_PRESS_PARAMS F1					 ; // VK_F1						, 0x70
	INPUT_PRESS_PARAMS F2					 ; // VK_F2						, 0x71
	INPUT_PRESS_PARAMS F3					 ; // VK_F3						, 0x72
	INPUT_PRESS_PARAMS F4					 ; // VK_F4						, 0x73
	INPUT_PRESS_PARAMS F5					 ; // VK_F5						, 0x74
	INPUT_PRESS_PARAMS F6					 ; // VK_F6						, 0x75
	INPUT_PRESS_PARAMS F7					 ; // VK_F7						, 0x76
	INPUT_PRESS_PARAMS F8					 ; // VK_F8						, 0x77
	INPUT_PRESS_PARAMS F9					 ; // VK_F9						, 0x78
	INPUT_PRESS_PARAMS F10					 ; // VK_F10					, 0x79
	INPUT_PRESS_PARAMS F11					 ; // VK_F11					, 0x7A
	INPUT_PRESS_PARAMS F12					 ; // VK_F12					, 0x7B
	INPUT_PRESS_PARAMS F13					 ; // VK_F13					, 0x7C
	INPUT_PRESS_PARAMS F14					 ; // VK_F14					, 0x7D
	INPUT_PRESS_PARAMS F15					 ; // VK_F15					, 0x7E
	INPUT_PRESS_PARAMS F16					 ; // VK_F16					, 0x7F
	INPUT_PRESS_PARAMS F17					 ; // VK_F17					, 0x80
	INPUT_PRESS_PARAMS F18					 ; // VK_F18					, 0x81
	INPUT_PRESS_PARAMS F19					 ; // VK_F19					, 0x82
	INPUT_PRESS_PARAMS F20					 ; // VK_F20					, 0x83
	INPUT_PRESS_PARAMS F21					 ; // VK_F21					, 0x84
	INPUT_PRESS_PARAMS F22					 ; // VK_F22					, 0x85
	INPUT_PRESS_PARAMS F23					 ; // VK_F23					, 0x86
	INPUT_PRESS_PARAMS F24					 ; // VK_F24					, 0x87
	INPUT_PRESS_PARAMS NumLock				 ; // VK_NUMLOCK				, 0x90
	INPUT_PRESS_PARAMS Scroll				 ; // VK_SCROLL					, 0x91
	INPUT_PRESS_PARAMS LeftShift			 ; // VK_LSHIFT					, 0xA0
	INPUT_PRESS_PARAMS RightShift			 ; // VK_RSHIFT					, 0xA1
	INPUT_PRESS_PARAMS LeftControl			 ; // VK_LCONTROL				, 0xA2
	INPUT_PRESS_PARAMS RightControl			 ; // VK_RCONTROL				, 0xA3
	INPUT_PRESS_PARAMS LeftAlt				 ; // VK_LMENU					, 0xA4
	INPUT_PRESS_PARAMS RightAlt				 ; // VK_RMENU					, 0xA5
	INPUT_PRESS_PARAMS BrowserBack			 ; // VK_BROWSER_BACK			, 0xA6
	INPUT_PRESS_PARAMS BrowserForward		 ; // VK_BROWSER_FORWARD			, 0xA7
	INPUT_PRESS_PARAMS BrowserRefresh		 ; // VK_BROWSER_REFRESH			, 0xA8
	INPUT_PRESS_PARAMS BrowserStop			 ; // VK_BROWSER_STOP			, 0xA9
	INPUT_PRESS_PARAMS BrowserSearch		 ; // VK_BROWSER_SEARCH	  		, 0xAA
	INPUT_PRESS_PARAMS BrowserFavorites		 ; // VK_BROWSER_FAVORITES		, 0xAB
	INPUT_PRESS_PARAMS BrowserHome			 ; // VK_BROWSER_HOME			, 0xAC
	INPUT_PRESS_PARAMS VolumeMute			 ; // VK_VOLUME_MUTE			, 0xAD
	INPUT_PRESS_PARAMS VolumeDown			 ; // VK_VOLUME_DOWN				, 0xAE
	INPUT_PRESS_PARAMS VolumeUp				 ; // VK_VOLUME_UP				, 0xAF
	INPUT_PRESS_PARAMS MediaNextTrack		 ; // VK_MEDIA_NEXT_TRACK		, 0xB0
	INPUT_PRESS_PARAMS MediaPreviousTrack	 ; // VK_MEDIA_PREV_TRACK		, 0xB1
	INPUT_PRESS_PARAMS MediaStop			 ; // VK_MEDIA_STOP		  		, 0xB2
	INPUT_PRESS_PARAMS MediaPlayPause		 ; // VK_MEDIA_PLAY_PAUSE		, 0xB3
	INPUT_PRESS_PARAMS LaunchMail			 ; // VK_LAUNCH_MAIL			, 0xB4
	INPUT_PRESS_PARAMS SelectMedia			 ; // VK_LAUNCH_MEDIA_SELECT	, 0xB5
	INPUT_PRESS_PARAMS LaunchApplication1	 ; // VK_LAUNCH_APP1			, 0xB6
	INPUT_PRESS_PARAMS LaunchApplication2	 ; // VK_LAUNCH_APP2			, 0xB7
	INPUT_PRESS_PARAMS OemSemicolon			 ; // VK_OEM_1					, 0xBA
	INPUT_PRESS_PARAMS OemPlus				 ; // VK_OEM_PLUS				, 0xBB
	INPUT_PRESS_PARAMS OemComma				 ; // VK_OEM_COMMA				, 0xBC
	INPUT_PRESS_PARAMS OemMinus				 ; // VK_OEM_MINUS				, 0xBD
	INPUT_PRESS_PARAMS OemPeriod			 ; // VK_OEM_PERIOD				, 0xBE
	INPUT_PRESS_PARAMS OemQuestion			 ; // VK_OEM_2					, 0xBF
	INPUT_PRESS_PARAMS OemTilde				 ; // VK_OEM_3					, 0xC0
	INPUT_PRESS_PARAMS OemOpenBrackets		 ; // VK_OEM_4					, 0xDB
	INPUT_PRESS_PARAMS OemPipe				 ; // VK_OEM_5					, 0xDC
	INPUT_PRESS_PARAMS OemCloseBrackets		 ; // VK_OEM_6					, 0xDD
	INPUT_PRESS_PARAMS OemQuotes			 ; // VK_OEM_7					, 0xDE
	INPUT_PRESS_PARAMS Oem8					 ; // VK_OEM_8					, 0xDF
	INPUT_PRESS_PARAMS OemBackslash			 ; // VK_OEM_102				, 0xE2
	INPUT_PRESS_PARAMS ProcessKey			 ; // VK_PROCESSKEY				, 0xE5
	INPUT_PRESS_PARAMS OemCopy				 ; //							  0XF2
	INPUT_PRESS_PARAMS OemAuto				 ; //							  0xF3
	INPUT_PRESS_PARAMS OemEnlW				 ; //							  0xF4
	INPUT_PRESS_PARAMS Attn					 ; // VK_ATTN					, 0xF6
	INPUT_PRESS_PARAMS Crsel				 ; // VK_CRSEL					, 0xF7
	INPUT_PRESS_PARAMS Exsel				 ; // VK_EXSEL					, 0xF8
	INPUT_PRESS_PARAMS EraseEof				 ; // VK_EREOF					, 0xF9
	INPUT_PRESS_PARAMS Play					 ; // VK_PLAY					, 0xFA
	INPUT_PRESS_PARAMS Zoom					 ; // VK_ZOOM					, 0xFB
	INPUT_PRESS_PARAMS Pa1					 ; // VK_PA1					, 0xFD
	INPUT_PRESS_PARAMS OemClear				 ; // VK_OEM_CLEAR				, 0xFE
	};
};

#pragma endregion Keyboard

#pragma region Gamepad

struct GAMEPAD_STICK_TRIGGER_PARAMS
{
	float val;
	float old;
	float delta;
};

struct GAMEPAD_BUTTON_DATA
{
	INPUT_PRESS_PARAMS A		;// A button
	INPUT_PRESS_PARAMS B		;// B button
	INPUT_PRESS_PARAMS X		;// X button
	INPUT_PRESS_PARAMS Y		;// Y button
	INPUT_PRESS_PARAMS Lshoulder;// L1
	INPUT_PRESS_PARAMS Rshoulder;// R1
	INPUT_PRESS_PARAMS Lstick	;// L3
	INPUT_PRESS_PARAMS Rstick	;// R3

	INPUT_PRESS_PARAMS LT_up	;// L2 throttle < 0.5
	INPUT_PRESS_PARAMS RT_up	;// R2 throttle < 0.5

	INPUT_PRESS_PARAMS LS_up	;// L.y stick throttle < 0.5
	INPUT_PRESS_PARAMS LS_down	;// L.y stick throttle < -0.5
	INPUT_PRESS_PARAMS LS_right	;// L.x stick throttle < 0.5
	INPUT_PRESS_PARAMS LS_left	;// L.x stick throttle < -0.5
	INPUT_PRESS_PARAMS RS_up	;// R.y stick throttle < 0.5
	INPUT_PRESS_PARAMS RS_down	;// R.y stick throttle < -0.5
	INPUT_PRESS_PARAMS RS_right	;// R.x stick throttle < 0.5
	INPUT_PRESS_PARAMS RS_left	;// R.x stick throttle < -0.5

	union
	{
		INPUT_PRESS_PARAMS Back;
		INPUT_PRESS_PARAMS View;
	};
	union
	{
		INPUT_PRESS_PARAMS Start;
		INPUT_PRESS_PARAMS Menu;
	};
};

struct GAMEPAD_DPAD_DATA
{
	INPUT_PRESS_PARAMS Up;
	INPUT_PRESS_PARAMS Down;
	INPUT_PRESS_PARAMS Right;
	INPUT_PRESS_PARAMS Left;
};

struct GAMEPAD_TRIGGER_DATA
{
	GAMEPAD_STICK_TRIGGER_PARAMS LT;
	GAMEPAD_STICK_TRIGGER_PARAMS RT;
};

struct GAMEPAD_STICK_DATA
{
	GAMEPAD_STICK_TRIGGER_PARAMS LS_x;
	GAMEPAD_STICK_TRIGGER_PARAMS LS_y;
	GAMEPAD_STICK_TRIGGER_PARAMS RS_x;
	GAMEPAD_STICK_TRIGGER_PARAMS RS_y;
};

enum eGPDeadZone
{
	DEAD_ZONE_INDEPENDENT_AXES = 0
	, DEAD_ZONE_CIRCULAR
	, DEAD_ZONE_NONE
};

struct GAMEPAD_DATA
{
	GAMEPAD_BUTTON_DATA  buttons;
	GAMEPAD_DPAD_DATA 	 dpad;
	GAMEPAD_TRIGGER_DATA trig;
	GAMEPAD_STICK_DATA 	 stick;
	bool is_connected;
	eGPDeadZone dead_zone;
};

#pragma endregion Gamepad

}
}
