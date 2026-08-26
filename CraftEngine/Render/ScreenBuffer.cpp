#include <Render/ScreenBuffer.h>

#include <Math/Color.h>

#include <cassert>

namespace Craft
{
	ScreenBuffer::ScreenBuffer(const Vector2I& screenSize)
		: screenSize(screenSize)
	{
		// 콘솔 버퍼 생성.
		screenBuffer = CreateConsoleScreenBuffer(
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			nullptr,
			CONSOLE_TEXTMODE_BUFFER,
			nullptr
			);
		
		// 제대로 생성됐는지 확인.
		assert(screenBuffer != INVALID_HANDLE_VALUE && "Console Buffer Invalid");

		BOOL result = SetConsoleScreenBufferSize(screenBuffer, screenSize);

		assert(result == TRUE);

		// 화면 창 크기 설정.
		SMALL_RECT rect = {};
		rect.Top = 0;
		rect.Left = 0;
		rect.Right = static_cast<short>(screenSize.x - 1);
		rect.Bottom = static_cast<short>(screenSize.y - 1);

		result = SetConsoleWindowInfo(screenBuffer, TRUE, &rect);
		assert(result == TRUE);

		// 커서 끄기(커서 깜빡임 방지).
		CONSOLE_CURSOR_INFO info;
		result = GetConsoleCursorInfo(screenBuffer, &info);
		assert(result == TRUE);

		info.bVisible = FALSE;
		SetConsoleCursorInfo(screenBuffer, &info);
		assert(result == TRUE);
	}

	ScreenBuffer::~ScreenBuffer()
	{
		// 버퍼 해제.
		if (screenBuffer)
		{
			CloseHandle(screenBuffer);
		}
	}

	void ScreenBuffer::Clear() const
	{
		CONSOLE_SCREEN_BUFFER_INFO bufferInfo{};

		if (!GetConsoleScreenBufferInfo(
			screenBuffer,
			&bufferInfo))
		{
			return;
		}

		const DWORD cellCount =
			static_cast<DWORD>(bufferInfo.dwSize.X) *
			static_cast<DWORD>(bufferInfo.dwSize.Y);

		// 콘솔을 지우는 함수.
		// 공백 문자를 화면 크기 전체에 한번에 설정.
		
		// 글자를 채울 시작 위치.
		COORD coord = {};
		coord.X = 0;
		coord.Y = 0;

		// 화면에 설정된 글자 수 (출력용)
		DWORD writtenCount = 0;

		BOOL result = FillConsoleOutputCharacterA(
			screenBuffer,
			' ',
			cellCount,
			coord,
			&writtenCount
		);

		FillConsoleOutputAttribute(
			screenBuffer,
			0,
			cellCount,
			coord,
			&writtenCount
		);

		assert(result == TRUE);
	}

	void ScreenBuffer::Draw(const CHAR_INFO* const charInfo) const
	{
		// 설정할 글자 영역.
		SMALL_RECT writeRegion = {
			0,										// Left
			0,										// Top
			static_cast<short>(screenSize.x - 1),	// Right
			static_cast<short>(screenSize.y - 1)	// Bottom
		};

		BOOL result = WriteConsoleOutputA(
			screenBuffer,
			charInfo,
			screenSize,
			Vector2I::Zero,
			&writeRegion
		);

		// 제대로 실행됐는지 확인 여부
		assert(result == TRUE);
	}
}