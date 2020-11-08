#include "pch.h"
#include "PCInputs.h"
#include "MouseInput.h"
#include "GamepadInputs.h"
#include "KeyboardInput.h"

namespace buma
{
namespace input
{

const KEY_PRESS_DATA&   GetKey()                    { return buma::input::KeyboardInput::GetIns().KeyPressData(); }
const MOUSE_DATA&       GetMouse()                  { return buma::input::MouseInput::GetIns().MouseData(); }
const GAMEPAD_DATA&     GetGpad(int _player)        { return buma::input::GamePadInputs::GetIns().GamePadData(_player); }

}// namespace input
}// namespace buma
