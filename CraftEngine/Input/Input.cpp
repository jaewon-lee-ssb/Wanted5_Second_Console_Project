#include <Input/Input.h>

#include <cassert>
#include <Windows.h>

namespace Craft
{
	// 전역 변수 초기화
	Input* Input::instance = nullptr;

	Input::Input()
	{
		assert(!instance);
		instance = this;
	}

	bool Input::GetKeyDown(int keyCode) const
	{
		return keyStates[keyCode].isKeyDown && !keyStates[keyCode].wasKeyDown;
	}

	bool Input::GetKeyUp(int keyCode) const
	{
		return !keyStates[keyCode].isKeyDown && keyStates[keyCode].wasKeyDown;
	}

	bool Input::GetKey(int keyCode) const
	{
		return keyStates[keyCode].isKeyDown;
	}

	Input& Input::Get()
	{
		assert(instance);
		return *instance;
	}

	void Input::ProcessInput()
	{
		// 현재 프레임에 키 입력이 발생했는지 확인
		for (int i = 0; i < keyCount; ++i)
		{
			// 키 눌림 여부 저장.
			// 0x8000은 최상위 비트.
			keyStates[i].isKeyDown = ((GetAsyncKeyState(i) & 0x8000) != 0);
		}
	}

	void Input::SavePreviousStates()
	{
		for (KeyState& state : keyStates)
		{
			state.wasKeyDown = state.isKeyDown;
		}
	}
}