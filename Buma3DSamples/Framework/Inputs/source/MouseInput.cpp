#include "pch.h"
#include "MouseInput.h"

namespace buma
{
namespace input
{

void MouseInput::ProcessMessage(UINT _message, WPARAM _wparam, LPARAM _lparam)
{
	DirectX::Mouse::ProcessMessage(_message, _wparam, _lparam);
}

void MouseInput::Update(float _delta_time)
{
	auto state = mouse->GetState();

	UpdatePressData(mouse_data.buttons.left		, state.leftButton	, _delta_time);
	UpdatePressData(mouse_data.buttons.middle	, state.middleButton, _delta_time);
	UpdatePressData(mouse_data.buttons.right	, state.rightButton	, _delta_time);
	UpdatePressData(mouse_data.buttons.x_button1, state.xButton1	, _delta_time);
	UpdatePressData(mouse_data.buttons.x_button2, state.xButton2	, _delta_time);

	UpdatePosParams(mouse_data.x	, state.x);
	UpdatePosParams(mouse_data.y	, state.y);
	UpdatePosParams(mouse_data.rot	, state.scrollWheelValue);
	mouse->ResetScrollWheelValue();
}

MouseInput::MouseInput() : mouse_data{}
{
	mouse = std::make_unique < DirectX::Mouse>();
}
MouseInput::~MouseInput()
{
	mouse.reset();
}

}
}
