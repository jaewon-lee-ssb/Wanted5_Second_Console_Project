#include <Render/Renderer.h>
#include <Render/ScreenBuffer.h>
#include <Camera/Camera.h>

#include <cassert>
#include <Windows.h>
#include <iostream>
#include <vector>

namespace Craft
{
	// ------------------------ Frame ------------------------ // 
	Renderer::Frame::Frame(int bufferCount)
	{
		// 2차원 글자 배열 및 그리기 순서 배열 객체 생성
		charInfoArray = std::make_unique<CHAR_INFO[]>(bufferCount);
		sortingOrderArray = std::make_unique<int[]>(bufferCount);
	}

	Renderer::Frame::~Frame()
	{

	}

	void Renderer::Frame::Clear(const Vector2I& screenSize)
	{
		// 이중 루프를 순회하면서 값 정리
		const int width = screenSize.x;
		const int height = screenSize.y;

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				// 2차원 배열 인덱스(x, y)를 1차원 인덱스로 변환.
				const int index = (y * width) + x;
				
				// 글자 값 초기화.
				CHAR_INFO& info = charInfoArray[index];
				// 빈문자 설정 - 기존의 설정된 값 지우기
				info.Char.AsciiChar = ' ';
				// 색상 표기 안함
				info.Attributes = 0;

				// 그리기 순서 배열 항목 초기화
				sortingOrderArray[index] = -1;
			}
		}
	}
	// ------------------------ Frame ------------------------ // 

	// 전역 변수 초기화.
	Renderer* Renderer::instance = nullptr;

	Renderer::Renderer(const Vector2I& screenSize, const Vector2I& viewportSize)
		: screenSize(screenSize), viewportSize(viewportSize)
	{
		assert(!instance && "instance should be null");
		instance = this;

		// 프레임 생성
		const int bufferCount = screenSize.x * screenSize.y;
		frame = std::make_unique<Frame>(bufferCount);

		// 생성 후 프레임 지우기.
		frame->Clear(screenSize);

		// 이중 버퍼에 사용할 버퍼 생성.
		screenBufferArray[0] = std::make_unique<ScreenBuffer>(screenSize);
		screenBufferArray[0]->Clear();

		screenBufferArray[1] = std::make_unique<ScreenBuffer>(screenSize);
		screenBufferArray[1]->Clear();

		// 0번 콘솔 버퍼를 창에 설정
		SetConsoleActiveScreenBuffer(screenBufferArray[0]->GetScreenBuffer());
	}

	Renderer::~Renderer()
	{
		instance = nullptr;

		// 콘솔창 원래대로 복구.
		SetConsoleActiveScreenBuffer(GetStdHandle(STD_OUTPUT_HANDLE));
	}

	void Renderer::SubmitWorld(const std::string& image, const Vector2F& position, Color color, BackgroundColor backColor, const Vector2F& pivot, int sortingOrder, RenderSpace renderSpace)
	{
		SubmitWorld(std::vector<std::string>{ image }, position, color, backColor, pivot, sortingOrder, renderSpace);
	}

	void Renderer::SubmitWorld(const std::vector<std::string>& image, const Vector2F& position, Color color, BackgroundColor backColor, const Vector2F& pivot, int sortingOrder, RenderSpace renderSpace)
	{
		// 그럼 명령 객체 생성.
		RenderCommand command;
		command.image = image;
		command.position = position;
		command.color = color;
		command.backColor = backColor;
		command.pivot = pivot;
		command.sortingOrder = sortingOrder;
		command.renderSpace = renderSpace;

		// 렌더 큐에 명령 추가
		renderQueue.emplace_back(command);
	}

	void Renderer::SubmitUI(const std::string& image, const Vector2F& position, Color color, BackgroundColor backColor, const Vector2F& pivot, int sortingOrder, RenderSpace renderSpace)
	{
		SubmitUI(std::vector<std::string>{ image }, position, color, backColor, pivot, sortingOrder, renderSpace);
	}

	void Renderer::SubmitUI(const std::vector<std::string>& image, const Vector2F& position, Color color, BackgroundColor backColor, const Vector2F& pivot, int sortingOrder, RenderSpace renderSpace)
	{
		// 그럼 명령 객체 생성.
		RenderCommand command;
		command.image = image;
		command.position = position;
		command.color = color;
		command.backColor = backColor;
		command.pivot = pivot;
		command.sortingOrder = sortingOrder;
		command.renderSpace = renderSpace;

		// 렌더 큐에 명령 추가
		renderQueue.emplace_back(command);
	}

	void Renderer::Draw()
	{
		// 화면 지우기.
		Clear();

		// 프레임 그리기.
		DrawRenderQueue();

		// 버퍼 교환(이중 버퍼).
		Present();
	}

	Renderer& Renderer::Get()
	{
		assert(instance && "instance should not be null");
		return *instance;
	}

	void Renderer::Clear()
	{
		// 프레임 초기화.
		frame->Clear(screenSize);

		// 콘솔 버퍼 초기화
		GetCurrentBuffer()->Clear();

	}

	void Renderer::DrawRenderQueue()
	{
		// 렌더 큐 순회
		for (const RenderCommand& command : renderQueue)
		{
			// 그릴 문자열이 없으면 건너뛰기.
			if (command.image.empty())
			{
				continue;
			}

			const Vector2I& drawSize = command.renderSpace == RenderSpace::World ? viewportSize : screenSize;

			const int startX = static_cast<int>(std::floor(command.position.x - command.pivot.x));
			const int startY = static_cast<int>(std::floor(command.position.y - command.pivot.y));

			for (int localY = 0; localY < static_cast<int>(command.image.size()); ++localY)
			{
				const std::string& row = command.image[localY];
				const int screenY = startY + localY;

				if (screenY < 0 || screenY >= drawSize.y)
				{
					continue;
				}

				for (int localX = 0; localX < static_cast<int>(row.length()); ++localX)
				{
					const int screenX = startX + localX;

					if (screenX < 0 || screenX >= drawSize.x)
					{
						continue;
					}

					char ch = row[localX];

					if (ch == ' ')
					{
						continue;
					}

					const int index = (screenY * screenSize.x) + screenX;

					if (frame->sortingOrderArray[index] > command.sortingOrder)
					{
						continue;
					}

					frame->charInfoArray[index].Char.AsciiChar = ch;
					frame->charInfoArray[index].Attributes = static_cast<WORD>(command.color) | static_cast<WORD>(command.backColor);
					frame->sortingOrderArray[index] = command.sortingOrder;
				}
			}
		}

		// 현재 백버퍼(현재 콘솔에서 사용하지 않는 버퍼)에 2차원 배열 기록.
		GetCurrentBuffer()->Draw(frame->charInfoArray.get());

		// 렌더 큐 비우기.
		renderQueue.clear();

		// 콘솔 초기화.
		SetConsoleTextAttribute(GetCurrentBuffer()->GetScreenBuffer(), static_cast<WORD>(Color::White));
	}

	void Renderer::Present()
	{
		// 현재 백버퍼를 화면에 활성화.
		SetConsoleActiveScreenBuffer(GetCurrentBuffer()->GetScreenBuffer());

		// 버퍼 순번 교환. 1 -> 0, 0 -> 1, 1 -> 0...
		// One Minus X. (1-x)
		currentBufferIndex = 1 - currentBufferIndex;
	}

	const ScreenBuffer* const Renderer::GetCurrentBuffer() const
	{
		return screenBufferArray[currentBufferIndex].get();
	}

}