#pragma once
#include "KeyboardInput.h"
#include "MouseInput.h"
#include "GamepadInputs.h"

//#define KEY   buma::input::KeyboardInput::GetIns().KeyPressData()
//#define MOUSE buma::input::MouseInput::GetIns().MouseData()
//#define GPADS buma::input::GamePadInputs::GetIns().GamePadData

namespace buma
{
namespace input
{

const KEY_PRESS_DATA&   GetKey()                    { return buma::input::KeyboardInput::GetIns().KeyPressData(); }
const MOUSE_DATA&       GetMouse()                  { return buma::input::MouseInput::GetIns().MouseData(); }
const GAMEPAD_DATA&     GetGpad(int _player = 0)    { return buma::input::GamePadInputs::GetIns().GamePadData(_player); }

}// namespace input
}// namespace buma
