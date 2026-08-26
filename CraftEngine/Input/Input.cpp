#include <Input/Input.h>

#include <cassert>
#include <Windows.h>

namespace Craft
{
	// 전역 변수 초기화
	Input* Input::instance = nullptr;

	Input::Input()
	{
		// 시작할 때 instance 값은 null이어야 함.
		assert(!instance && "instance should be null here.");
		instance = this;

		// 콘솔 입력 버퍼의 핸들을 가져옴.
		inputHandle = GetStdHandle(STD_INPUT_HANDLE);

		// 콘솔 입력 핸들을 가져오지 못했으면 종료.
		if (inputHandle == INVALID_HANDLE_VALUE || inputHandle == nullptr)
		{
			return;
		}

		// 현재 콘솔 입력 모드를 가져옴.
		if (GetConsoleMode(inputHandle, &originalConsoleMode))
		{
			// 변경할 콘솔 입력 모드 설정.
			DWORD inputMode = originalConsoleMode;

			// 마우스 이벤트 입력 활성화.
			inputMode |= ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT;

			// 빠른 편집 모드가 활성화되어 있으면 마우스 입력이
			// 콘솔의 드래그 선택 기능으로 처리되므로 비활성화.
			inputMode &= ~ENABLE_QUICK_EDIT_MODE;

			// 변경한 콘솔 입력 모드를 적용하고 성공 여부 저장.
			shouldRestoreConsoleMode = SetConsoleMode(inputHandle, inputMode) != FALSE;
		}

		// 렌더링 중에도 콘솔 입력을 놓치지 않도록 입력 전용 스레드 시작.
		inputThread = std::thread(&Input::InputThreadLoop, this);
	}

	Input::~Input()
	{
		// 입력 스레드를 먼저 종료한 뒤 콘솔 입력 모드를 복구.
		shouldStopInputThread.store(true);
		if (inputThread.joinable())
		{
			inputThread.join();
		}

		// 입력 모드 변경에 성공했으면 기존 콘솔 입력 모드로 복구.
		if (shouldRestoreConsoleMode)
		{
			SetConsoleMode(inputHandle, originalConsoleMode);
		}

		// 전역 접근 변수 정리.
		instance = nullptr;
	}

	bool Input::GetKeyDown(int keyCode) const
	{
		return keyStates[keyCode].pressedThisFrame;
	}

	bool Input::GetKeyUp(int keyCode) const
	{
		return keyStates[keyCode].releasedThisFrame;
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
		// 입력 스레드가 모아둔 이벤트를 잠금 시간을 짧게 유지하며 가져옴.
		std::vector<INPUT_RECORD> records;
		{
			std::lock_guard<std::mutex> lock(pendingInputMutex);
			records.swap(pendingInputRecords);
		}

		// 읽은 입력 이벤트를 순서대로 처리.
		for (const INPUT_RECORD& record : records)
		{
			// 입력 이벤트 종류에 따라 처리.
			switch (record.EventType)
			{
			case KEY_EVENT:
			{
				// 키보드 이벤트 정보 가져오기.
				const KEY_EVENT_RECORD& keyEvent = record.Event.KeyEvent;

				// 입력된 키의 가상 키 코드 가져오기.
				const WORD keyCode = keyEvent.wVirtualKeyCode;

				// 관리하는 키 배열 범위 안에 있는지 확인.
				if (keyCode < keyCount)
				{
					// 키가 눌렸는지 또는 해제됐는지 현재 상태에 저장.
					const bool isKeyDown = keyEvent.bKeyDown != FALSE;
					UpdateKeyState(keyCode, isKeyDown);
				}
				break;
			}

			case MOUSE_EVENT:
			{
				// 마우스 이벤트 정보 가져오기.
				const MOUSE_EVENT_RECORD& mouseEvent = record.Event.MouseEvent;

				// 마우스 포인터의 콘솔 셀 좌표 저장.
				mousePosition.x = mouseEvent.dwMousePosition.X;
				mousePosition.y = mouseEvent.dwMousePosition.Y;

				// 마우스 버튼과 가상 키 코드를 연결하기 위한 구조체.
				const struct MouseButton
				{
					int keyCode;
					DWORD buttonMask;
				} mouseButtons[] = {
					{ VK_LBUTTON, FROM_LEFT_1ST_BUTTON_PRESSED },
					{ VK_RBUTTON, RIGHTMOST_BUTTON_PRESSED },
					{ VK_MBUTTON, FROM_LEFT_2ND_BUTTON_PRESSED }
				};

				for (const MouseButton& button : mouseButtons)
				{
					const bool isKeyDown =
						(mouseEvent.dwButtonState & button.buttonMask) != 0;
					UpdateKeyState(button.keyCode, isKeyDown);
				}
				break;
			}

			case FOCUS_EVENT:
				// 콘솔 창이 입력 포커스를 잃었는지 확인.
				if (!record.Event.FocusEvent.bSetFocus)
				{
					for (KeyState& state : keyStates)
					{
						state.isKeyDown = false;
						state.pressedThisFrame = false;
						state.releasedThisFrame = false;
					}
				}
				break;
			}
		}
	}

	void Input::InputThreadLoop()
	{
		while (!shouldStopInputThread.load())
		{
			// 입력이 생기거나 종료 확인 시간이 될 때까지 짧게 대기.
			const DWORD waitResult = WaitForSingleObject(inputHandle, 5);
			if (waitResult == WAIT_TIMEOUT)
			{
				continue;
			}
			if (waitResult != WAIT_OBJECT_0)
			{
				break;
			}

			INPUT_RECORD records[128] = {};
			DWORD readEventCount = 0;
			if (!ReadConsoleInput(inputHandle, records, 128, &readEventCount))
			{
				break;
			}

			std::lock_guard<std::mutex> lock(pendingInputMutex);
			pendingInputRecords.insert(
				pendingInputRecords.end(),
				records,
				records + readEventCount
			);
		}
	}

	void Input::SavePreviousStates()
	{
		// 이전 프레임 입력 값 저장.
		for (KeyState& state : keyStates)
		{
			// 현재 프레임 입력 값을 이전 프레임 값으로 저장.
			state.pressedThisFrame = false;
			state.releasedThisFrame = false;
		}
	}
	void Input::UpdateKeyState(int keyCode, bool newKeyDown)
	{
		KeyState& state = keyStates[keyCode];

		// 실제 상태가 변하지 않았다면 자동 반복 입력들은 무시
		if (state.isKeyDown == newKeyDown)
		{
			return;
		}

		state.isKeyDown = newKeyDown;

		if (newKeyDown)
		{
			state.pressedThisFrame = true;
		}
		else
		{
			state.releasedThisFrame = true;
		}


	}
}
