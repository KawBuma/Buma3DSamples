#pragma once
#include "InputStructs.h"

//#define KEY   buma::input::KeyboardInput::GetIns().KeyPressData()
//#define MOUSE buma::input::MouseInput::GetIns().MouseData()
//#define GPADS buma::input::GamePadInputs::GetIns().GamePadData

namespace buma
{
namespace input
{

const KEY_PRESS_DATA& GetKey();
const MOUSE_DATA& GetMouse();
const GAMEPAD_DATA& GetGpad(int _player = 0);

}// namespace input
}// namespace buma
