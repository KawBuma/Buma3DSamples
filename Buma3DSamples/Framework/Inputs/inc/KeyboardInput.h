#pragma once
#include <memory>
#include "Keyboard.h"
#include "InputStructs.h"

namespace buma
{
namespace input
{

enum eKeys
{
	  Back = 0
	, Tab

	, Enter

	, Pause
	, CapsLock
	, Kana

	, Kanji

	, Escape
	, ImeConvert
	, ImeNoConvert

	, Space
	, PageUp
	, PageDown
	, End
	, Home
	, Left
	, Up
	, Right
	, Down
	, Select
	, Print
	, Execute
	, PrintScreen
	, Insert
	, Delete
	, Help
	, D0
	, D1
	, D2
	, D3
	, D4
	, D5
	, D6
	, D7
	, D8
	, D9

	, A
	, B
	, C
	, D
	, E
	, F
	, G
	, H
	, I
	, J
	, K
	, L
	, M
	, N
	, O
	, P
	, Q
	, R
	, S
	, T
	, U
	, V
	, W
	, X
	, Y
	, Z
	, LeftWindows
	, RightWindows
	, Apps

	, Sleep
	, NumPad0
	, NumPad1
	, NumPad2
	, NumPad3
	, NumPad4
	, NumPad5
	, NumPad6
	, NumPad7
	, NumPad8
	, NumPad9
	, Multiply
	, Add
	, Separator
	, Subtract

	, Decimal
	, Divide
	, F1
	, F2
	, F3
	, F4
	, F5
	, F6
	, F7
	, F8
	, F9
	, F10
	, F11
	, F12
	, F13
	, F14
	, F15
	, F16
	, F17
	, F18
	, F19
	, F20
	, F21
	, F22
	, F23
	, F24

	, NumLock
	, Scroll

	, LeftShift
	, RightShift
	, LeftControl
	, RightControl
	, LeftAlt
	, RightAlt
	, BrowserBack
	, BrowserForward
	, BrowserRefresh
	, BrowserStop
	, BrowserSearch
	, BrowserFavorites
	, BrowserHome
	, VolumeMute
	, VolumeDown
	, VolumeUp
	, MediaNextTrack
	, MediaPreviousTrack
	, MediaStop
	, MediaPlayPause
	, LaunchMail
	, SelectMedia
	, LaunchApplication1
	, LaunchApplication2

	, OemSemicolon
	, OemPlus
	, OemComma
	, OemMinus
	, OemPeriod
	, OemQuestion
	, OemTilde

	, OemOpenBrackets
	, OemPipe
	, OemCloseBrackets
	, OemQuotes
	, Oem8

	, OemBackslash

	, ProcessKey

	, OemCopy
	, OemAuto
	, OemEnlW

	, Attn
	, Crsel
	, Exsel
	, EraseEof
	, Play
	, Zoom

	, Pa1
	, OemClear

	, Keys_End
};
union KEY_PRESS_DATA
{
	INPUT_PRESS_PARAMS press_params[eKeys::Keys_End];
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

class KeyboardInput
{
private:
	KeyboardInput();
	~KeyboardInput();

public:

	static KeyboardInput& GetIns()
	{
		static KeyboardInput ins;
		return ins;
	}
	
	static void ProcessMessage(UINT _message, WPARAM _wparam, LPARAM _lparam);

	void Update(float _delta_time);

	KEY_PRESS_DATA& KeyPressData() { return key_press_data; }

private:
	std::unique_ptr<DirectX::Keyboard> keys;
	KEY_PRESS_DATA key_press_data;

	static constexpr int keys_map_val_for_enum[Keys_End] =
	{
		  0x8	// Back
		, 0x9	// Tab
		  
		, 0xd	// Enter
		  
		, 0x13	// Pause
		, 0x14	// CapsLock
		, 0x15	// Kana
		  
		, 0x19	// Kanji
		  
		, 0x1b	// Escape
		, 0x1c	// ImeConvert
		, 0x1d	// ImeNoConvert
		  
		, 0x20	// Space
		, 0x21	// PageUp
		, 0x22	// PageDown
		, 0x23	// End
		, 0x24	// Home
		, 0x25	// Left
		, 0x26	// Up
		, 0x27	// Right
		, 0x28	// Down
		, 0x29	// Select
		, 0x2a	// Print
		, 0x2b	// Execute
		, 0x2c	// PrintScreen
		, 0x2d	// Insert
		, 0x2e	// Delete
		, 0x2f	// Help
		, 0x30	// D0
		, 0x31	// D1
		, 0x32	// D2
		, 0x33	// D3
		, 0x34	// D4
		, 0x35	// D5
		, 0x36	// D6
		, 0x37	// D7
		, 0x38	// D8
		, 0x39	// D9
		  
		, 0x41	// A
		, 0x42	// B
		, 0x43	// C
		, 0x44	// D
		, 0x45	// E
		, 0x46	// F
		, 0x47	// G
		, 0x48	// H
		, 0x49	// I
		, 0x4a	// J
		, 0x4b	// K
		, 0x4c	// L
		, 0x4d	// M
		, 0x4e	// N
		, 0x4f	// O
		, 0x50	// P
		, 0x51	// Q
		, 0x52	// R
		, 0x53	// S
		, 0x54	// T
		, 0x55	// U
		, 0x56	// V
		, 0x57	// W
		, 0x58	// X
		, 0x59	// Y
		, 0x5a	// Z
		, 0x5b	// LeftWindows
		, 0x5c	// RightWindows
		, 0x5d	// Apps
		  
		, 0x5f	// Sleep
		, 0x60	// NumPad0
		, 0x61	// NumPad1
		, 0x62	// NumPad2
		, 0x63	// NumPad3
		, 0x64	// NumPad4
		, 0x65	// NumPad5
		, 0x66	// NumPad6
		, 0x67	// NumPad7
		, 0x68	// NumPad8
		, 0x69	// NumPad9
		, 0x6a	// Multiply
		, 0x6b	// Add
		, 0x6c	// Separator
		, 0x6d	// Subtract
		  
		, 0x6e	// Decimal
		, 0x6f	// Divide
		, 0x70	// F1
		, 0x71	// F2
		, 0x72	// F3
		, 0x73	// F4
		, 0x74	// F5
		, 0x75	// F6
		, 0x76	// F7
		, 0x77	// F8
		, 0x78	// F9
		, 0x79	// F10
		, 0x7a	// F11
		, 0x7b	// F12
		, 0x7c	// F13
		, 0x7d	// F14
		, 0x7e	// F15
		, 0x7f	// F16
		, 0x80	// F17
		, 0x81	// F18
		, 0x82	// F19
		, 0x83	// F20
		, 0x84	// F21
		, 0x85	// F22
		, 0x86	// F23
		, 0x87	// F24
		  
		, 0x90	// NumLock
		, 0x91	// Scroll
		  
		, 0xa0	// LeftShift
		, 0xa1	// RightShift
		, 0xa2	// LeftControl
		, 0xa3	// RightControl
		, 0xa4	// LeftAlt
		, 0xa5	// RightAlt
		, 0xa6	// BrowserBack
		, 0xa7	// BrowserForward
		, 0xa8	// BrowserRefresh
		, 0xa9	// BrowserStop
		, 0xaa	// BrowserSearch
		, 0xab	// BrowserFavorites
		, 0xac	// BrowserHome
		, 0xad	// VolumeMute
		, 0xae	// VolumeDown
		, 0xaf	// VolumeUp
		, 0xb0	// MediaNextTrack
		, 0xb1	// MediaPreviousTrack
		, 0xb2	// MediaStop
		, 0xb3	// MediaPlayPause
		, 0xb4	// LaunchMail
		, 0xb5	// SelectMedia
		, 0xb6	// LaunchApplication1
		, 0xb7	// LaunchApplication2
		  
		, 0xba	// OemSemicolon
		, 0xbb	// OemPlus
		, 0xbc	// OemComma
		, 0xbd	// OemMinus
		, 0xbe	// OemPeriod
		, 0xbf	// OemQuestion
		, 0xc0	// OemTilde
		  
		, 0xdb	// OemOpenBrackets
		, 0xdc	// OemPipe
		, 0xdd	// OemCloseBrackets
		, 0xde	// OemQuotes
		, 0xdf	// Oem8
		  
		, 0xe2	// OemBackslash
		  
		, 0xe5	// ProcessKey
		  
		, 0xf2	// OemCopy
		, 0xf3	// OemAuto
		, 0xf4	// OemEnlW
		  
		, 0xf6	// Attn
		, 0xf7	// Crsel
		, 0xf8	// Exsel
		, 0xf9	// EraseEof
		, 0xfa	// Play
		, 0xfb	// Zoom
		  
		, 0xfd	// Pa1
		, 0xfe	// OemClear
	};

};

}
}
