#include "pch.h"
#include "KeyboardInput.h"

namespace buma
{
namespace input
{

KeyboardInput::KeyboardInput()
    : keys              {}
    , key_press_data    {}
{
    keys = new DirectX::Keyboard;
}

KeyboardInput::~KeyboardInput()
{
    delete keys;
    keys = nullptr;
}

void KeyboardInput::ProcessMessage(UINT _message, WPARAM _wparam, LPARAM _lparam)
{
	DirectX::Keyboard::ProcessMessage(_message, _wparam, _lparam);
}

void KeyboardInput::Update(float _delta_time)
{
	int key_index = 0;
	for (auto& i : key_press_data.press_params)
	{
		if (keys->GetState().IsKeyDown((DirectX::Keyboard::Keys)keys_map_val_for_enum[key_index]) == true)
		{
			i.is_release	 = false;
			i.press_count	+= 1;
			i.press_time	+= _delta_time;
		}
		else
		{
			i.is_release	= i.press_count;
			i.press_count	= 0;
			i.press_time	= 0.f;
		}

		key_index++;
	}
}

}
}
