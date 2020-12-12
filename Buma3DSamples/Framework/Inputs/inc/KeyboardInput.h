#pragma once
#include <memory>
#include "Keyboard.h"
#include "InputStructs.h"

namespace buma
{
namespace input
{

class KeyboardInput
{
public:
	KeyboardInput();
	~KeyboardInput();

public:	
	void ProcessMessage(UINT _message, WPARAM _wparam, LPARAM _lparam);

	void Update(float _delta_time);

	KEY_PRESS_DATA& KeyPressData() { return key_press_data; }

private:
	DirectX::Keyboard*  keys;
	KEY_PRESS_DATA      key_press_data;

	static constexpr int keys_map_val_for_enum[eKeys_NumKeys] =
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
