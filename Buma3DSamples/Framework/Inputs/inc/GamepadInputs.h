#pragma once
#include <memory>
#include "GamePad.h"
#include "InputStructs.h"

namespace buma
{
namespace input
{


#pragma region structs

struct GAMEPAD_STICK_TRIGGER_PARAMS
{
	float val;
	float old;
	float delta;
};

struct GAMEPAD_BUTTON_DATA
{
	INPUT_PRESS_PARAMS A		;// A button
	INPUT_PRESS_PARAMS B		;// B button
	INPUT_PRESS_PARAMS X		;// X button
	INPUT_PRESS_PARAMS Y		;// Y button
	INPUT_PRESS_PARAMS Lshoulder;// L1
	INPUT_PRESS_PARAMS Rshoulder;// R1
	INPUT_PRESS_PARAMS Lstick	;// L3
	INPUT_PRESS_PARAMS Rstick	;// R3

	INPUT_PRESS_PARAMS LT_up	;// L2 throttle < 0.5
	INPUT_PRESS_PARAMS RT_up	;// R2 throttle < 0.5

	INPUT_PRESS_PARAMS LS_up	;// L.y stick throttle < 0.5
	INPUT_PRESS_PARAMS LS_down	;// L.y stick throttle < -0.5
	INPUT_PRESS_PARAMS LS_right	;// L.x stick throttle < 0.5
	INPUT_PRESS_PARAMS LS_left	;// L.x stick throttle < -0.5
	INPUT_PRESS_PARAMS RS_up	;// R.y stick throttle < 0.5
	INPUT_PRESS_PARAMS RS_down	;// R.y stick throttle < -0.5
	INPUT_PRESS_PARAMS RS_right	;// R.x stick throttle < 0.5
	INPUT_PRESS_PARAMS RS_left	;// R.x stick throttle < -0.5

	union
	{
		INPUT_PRESS_PARAMS Back;
		INPUT_PRESS_PARAMS View;
	};
	union
	{
		INPUT_PRESS_PARAMS Start;
		INPUT_PRESS_PARAMS Menu;
	};
};

struct GAMEPAD_DPAD_DATA
{
	INPUT_PRESS_PARAMS Up;
	INPUT_PRESS_PARAMS Down;
	INPUT_PRESS_PARAMS Right;
	INPUT_PRESS_PARAMS Left;
};

struct GAMEPAD_TRIGGER_DATA
{
	GAMEPAD_STICK_TRIGGER_PARAMS LT;
	GAMEPAD_STICK_TRIGGER_PARAMS RT;
};

struct GAMEPAD_STICK_DATA
{
	GAMEPAD_STICK_TRIGGER_PARAMS LS_x;
	GAMEPAD_STICK_TRIGGER_PARAMS LS_y;
	GAMEPAD_STICK_TRIGGER_PARAMS RS_x;
	GAMEPAD_STICK_TRIGGER_PARAMS RS_y;
};

enum eGPDeadZone;
struct GAMEPAD_DATA
{
	GAMEPAD_BUTTON_DATA  buttons;
	GAMEPAD_DPAD_DATA 	 dpad;
	GAMEPAD_TRIGGER_DATA trig;
	GAMEPAD_STICK_DATA 	 stick;
	bool is_connected;
	eGPDeadZone dead_zone;
};

#pragma endregion

enum eGPDeadZone
{
	DEAD_ZONE_INDEPENDENT_AXES = 0
	, DEAD_ZONE_CIRCULAR
	, DEAD_ZONE_NONE
};

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
