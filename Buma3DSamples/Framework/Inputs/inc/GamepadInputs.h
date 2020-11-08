#pragma once
#include <memory>
#include "GamePad.h"
#include "InputStructs.h"

namespace buma
{
namespace input
{

class GamePadInputs
{
private:
	GamePadInputs();
	~GamePadInputs();
public:

public:
	static GamePadInputs& GetIns()
	{
		static GamePadInputs ins;
		return ins;
	}

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
	inline static void UpdatePosParams(GAMEPAD_STICK_TRIGGER_PARAMS& _st, float _current_val)
	{
		_st.old   = _st.val;
		_st.val   = _current_val;
		_st.delta = _st.val - _st.old;
	}
	void Update(float _delta_time);

	DirectX::GamePad::Capabilities	GetGetCapabilities	(int _player)	{ return game_pad->GetCapabilities(_player); }
	GAMEPAD_DATA&					GamePadData			(int _player)	{ return gamepad_data[_player]; }

	bool							IsConnected			(int _player, eGPDeadZone _dead_zone) { return game_pad->GetState(_player, (DirectX::GamePad::DeadZone)_dead_zone).IsConnected(); }

	void							Resume				()				{ game_pad->Resume(); }
	void							Suspend				()				{ game_pad->Suspend(); }
	void							RegisterEvents		(void* _ctrl_changed, void* _user_changed)	{ game_pad->RegisterEvents(_ctrl_changed, _user_changed); }
	bool							SetVibration		(int _player, float _left_motor, float _right_motor, float _left_trigger, float _right_trigger) 
	{ return game_pad->SetVibration(_player, _left_motor, _right_motor, _left_trigger, _right_trigger); }

private:
	std::unique_ptr<DirectX::GamePad> game_pad;
	GAMEPAD_DATA gamepad_data[DirectX::GamePad::MAX_PLAYER_COUNT];

};

}
}
