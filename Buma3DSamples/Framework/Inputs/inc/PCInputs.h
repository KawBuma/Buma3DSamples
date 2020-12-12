#pragma once
#include "InputStructs.h"

//#define KEY   buma::input::KeyboardInput::GetIns().KeyPressData()
//#define MOUSE buma::input::MouseInput::GetIns().MouseData()
//#define GPADS buma::input::GamePadInputs::GetIns().GamePadData

namespace buma
{
namespace input
{

class PCInputs
{
public:
    PCInputs() {}
    virtual ~PCInputs() {}

    virtual const KEY_PRESS_DATA&   GetKey() = 0;
    virtual const MOUSE_DATA&       GetMouse() = 0;
    virtual const GAMEPAD_DATA&     GetGpad(int _player = 0) = 0;

private:

};


}// namespace input
}// namespace buma
