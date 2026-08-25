#pragma once

#include <Core/Core.h>
#include <Math/Vector2.h>

#include <Windows.h>

namespace Craft
{
	// 입력 관련 기능을 제공
	class CRAFT_API Input
	{
		// Engine 클래스에서만 특정 함수에 접근 가능하도록.
		friend class Engine;

		// 키 입력 상태 구조체.
		struct KeyState
		{
			// 현재 프레임에 눌렀는지 여부 플래그.
			bool isKeyDown = false;
			
			// 이번 프레임 사이에 눌림 이벤트가 발생했는지 여부 플래그
			bool pressedThisFrame = false;

			// 이번 프레임 사이에 해제 이벤트가 발생했는지 여부 플래그
			bool releasedThisFrame = false;
		};

	public:
		Input();
		~Input();

		// 키 눌림/해제 여부 확인 함수.
		// 이전 프레임에는 키가 안눌렀다가 현재 프레임에 눌렀을 때 1번만 호출.
		bool GetKeyDown(int keyCode) const;
		
		// 이전 프레임에는 키가 눌렀다가 현재 프레임에 키 입력이 해제되면 1번만 발동.
		bool GetKeyUp(int keyCode) const;

		// 현재 프레임에 입력이 눌리면 계속 호출.
		bool GetKey(int keyCode) const;

		// 현재 마우스 포인터의 콘솔 셀 좌표를 반환.
		const Vector2F& GetMousePosition() const { return mousePosition; }

		const Vector2F& GetMousePressedPosition() const { return mousePressedPosition; }

		// 싱글톤 접근 함수.
		static Input& Get();

	private:
		// 현재 프레임에 키 입력이 눌렸는지 여부 확인 함수.
		void ProcessInput();

		// 현재 프레임의 입력 상태를 이전 프레임 상태로 저장하는 함수.
		void SavePreviousStates();

		// 
		void UpdateKeyState(int keyCode, bool newKeyDown);

		void PollMouseButton(int keyCode);

	private:
		// 가상 키의 수.
		const int keyCount = 256;

		// 키 입력 상태 관리 배열.
		KeyState keyStates[256] = {};

		// 싱글톤 구현을 위한 전역 변수
		static Input* instance;

		// 콘솔 입력 이벤트를 읽기 위한 핸들.
		HANDLE inputHandle = INVALID_HANDLE_VALUE;

		// 프로그램 시작 시 설정되어 있던 콘솔 입력 모드.
		DWORD originalConsoleMode = 0;

		// 종료할 때 기존 콘솔 입력 모드를 복구할지 여부.
		bool shouldRestoreConsoleMode = false;

		// 현재 마우스 포인터의 콘솔 셀 좌표.
		Vector2F mousePosition = Vector2F::Zero;

		Vector2F mousePressedPosition = Vector2F::Zero;
	};
}

