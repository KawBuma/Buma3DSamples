#include "pch.h"
#include "GamepadInputs.h"

namespace buma
{
namespace input
{

GamePadInputs::GamePadInputs()
{
	game_pad = std::make_unique<DirectX::GamePad>();
}

GamePadInputs::~GamePadInputs()
{
	game_pad.reset();
}

void GamePadInputs::Update(float _delta_time)
{
	int cnt = 0;
	for (auto& i : gamepad_data)
	{
		auto state = game_pad->GetState(cnt, (DirectX::GamePad::DeadZone)i.dead_zone);

		if (state.IsConnected() == false)
		{
			i.is_connected = false;
			cnt++;
			continue;
		}
		else
			i.is_connected = true;

		// buttons
		{
			UpdatePressData(i.buttons.A			, state.buttons.a				, _delta_time);
			UpdatePressData(i.buttons.B			, state.buttons.b				, _delta_time);
			UpdatePressData(i.buttons.X			, state.buttons.x				, _delta_time);
			UpdatePressData(i.buttons.Y			, state.buttons.y				, _delta_time);
			UpdatePressData(i.buttons.Lshoulder	, state.buttons.leftShoulder	, _delta_time);
			UpdatePressData(i.buttons.Rshoulder	, state.buttons.rightShoulder	, _delta_time);
			UpdatePressData(i.buttons.Lstick	, state.buttons.leftStick		, _delta_time);
			UpdatePressData(i.buttons.Rstick	, state.buttons.rightStick		, _delta_time);
			// trigger
			UpdatePressData(i.buttons.LT_up		, state.IsLeftTriggerPressed()	, _delta_time);
			UpdatePressData(i.buttons.RT_up		, state.IsRightTriggerPressed()	, _delta_time);
			// stick
			UpdatePressData(i.buttons.LS_up		, state.IsLeftThumbStickUp()	, _delta_time);
			UpdatePressData(i.buttons.LS_down	, state.IsLeftThumbStickDown()	, _delta_time);
			UpdatePressData(i.buttons.LS_right	, state.IsLeftThumbStickRight()	, _delta_time);
			UpdatePressData(i.buttons.LS_left	, state.IsLeftThumbStickLeft()	, _delta_time);
			UpdatePressData(i.buttons.RS_up		, state.IsRightThumbStickUp()	, _delta_time);
			UpdatePressData(i.buttons.RS_down	, state.IsRightThumbStickDown()	, _delta_time);
			UpdatePressData(i.buttons.RS_right	, state.IsRightThumbStickRight(), _delta_time);
			UpdatePressData(i.buttons.RS_left	, state.IsRightThumbStickLeft()	, _delta_time);
			// back, start
			UpdatePressData(i.buttons.Back		, state.buttons.back			, _delta_time);
		  //UpdatePressData(i.buttons.View		, state.buttons.view			, _delta_time);// == state.buttons.back
			UpdatePressData(i.buttons.Start		, state.buttons.start			, _delta_time);
		  //UpdatePressData(i.buttons.Menu		, state.buttons.menu			, _delta_time);// == state.buttons.menu
		}
		// dpad
		{
			UpdatePressData(i.dpad.Up			, state.dpad.up					, _delta_time);
			UpdatePressData(i.dpad.Down			, state.dpad.down				, _delta_time);
			UpdatePressData(i.dpad.Right		, state.dpad.right				, _delta_time);
			UpdatePressData(i.dpad.Left			, state.dpad.left				, _delta_time);
		}
		// trig
		{
			UpdatePosParams(i.trig.LT, state.triggers.left);
			UpdatePosParams(i.trig.RT, state.triggers.right);
		}
		// stick
		{
			UpdatePosParams(i.stick.LS_x, state.thumbSticks.leftX);
			UpdatePosParams(i.stick.LS_y, state.thumbSticks.leftY);
			UpdatePosParams(i.stick.RS_x, state.thumbSticks.rightX);
			UpdatePosParams(i.stick.RS_y, state.thumbSticks.rightY);
		}

		cnt++;
	}
}

}
}