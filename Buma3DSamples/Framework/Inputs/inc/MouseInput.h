#pragma once

#include <memory>
#include "Mouse.h"
#include "InputStructs.h"

namespace buma
{
namespace input
{

class MouseInput
{
public:
    MouseInput();
    ~MouseInput();

public:
    void ProcessMessage(UINT _message, WPARAM _wparam, LPARAM _lparam);
    void Update(float _delta_time);

    MOUSE_DATA& MouseData() { return mouse_data; }

    DirectX::Mouse::Mode    GetMode     ()                            const { return mouse->GetState().positionMode; }
    bool                    IsConnected ()                            const { return mouse->IsConnected(); }
    bool                    IsVisible   ()                            const { return mouse->IsVisible(); }
    void                    SetMode     (DirectX::Mouse::Mode _mode)        { mouse->SetMode(_mode); }
    void                    SetVisible  (bool _visible)                     { mouse->SetVisible(_visible); }
    void                    SetWindow   (HWND _hwnd)                        { mouse->SetWindow(_hwnd); }

private:
    inline static void UpdatePressData(INPUT_PRESS_PARAMS& _data, bool _state, float _delta_time)
    {
        if (_state == true)
        {
            _data.is_release   = false;
            _data.press_count += 1;
            _data.press_time  += _delta_time;
        }
        else
        {
            _data.is_release  = (_data.press_count != 0);
            _data.press_count = 0;
            _data.press_time  = 0.f;
        }
    }
    inline static void UpdatePosParams(INPUT_POS_PARAMS& _pos, int _current_val)
    {
        _pos.old   = _pos.val;
        _pos.val   = _current_val;
        _pos.delta = _pos.val - _pos.old;
    }

private:
    DirectX::Mouse* mouse;
    MOUSE_DATA      mouse_data;

};

}
}
