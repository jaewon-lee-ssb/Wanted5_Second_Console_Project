#pragma once

#include <Core/Core.h>
#include <Math/Vector2.h>
#include <Math/Color.h>
#include <Resource/PixelImage.h>

#include <vector>
#include <string>
#include <memory>

namespace Craft
{
	// 전방 선언
	class  ScreenBuffer;

	// 화면에 그리는 기능을 제공.
	class CRAFT_API Renderer
	{
		enum class RenderSpace
		{
			World,
			Screen
		};

		// 화면에 그릴 데이터를 명령으로 모아둘 구조체
		struct RenderCommand
		{
			// 화면에 그릴 색상 이미지 값.
			//std::string image;
			//std::vector<std::string> image;
			const PixelImage* image = nullptr;

			// 위치.
			Vector2F position = Vector2F::Zero;

			// 피벗
			Vector2F pivot = Vector2F::Zero;

			// 색상.
			//Color color = Color::White;

			// 배경 색상.
			//BackgroundColor backColor = BackgroundColor::Black;

			// 그리기 정렬 순서. 값이 크면 우선순위가 높음.
			int sortingOrder = -1;

			RenderSpace renderSpace = RenderSpace::World;
		};

		// 화면 프레임 (글자 2차원 배열).
		struct Frame
		{
			// 화면에 가로 x 세로 크기를 배열의 크기로 설정.
			Frame(int bufferCount);
			~Frame();

			// 프레임 초기화 함수.
			void Clear(const Vector2I& screenSize);

			// 화면에 그릴 2차원 배열 문자값.
			std::unique_ptr <CHAR_INFO[]> charInfoArray;

			// 그리기 정렬 값 2차원 배열.
			std::unique_ptr<int[]> sortingOrderArray;
		};
		
	public:
		Renderer(const Vector2I& screenSize, const Vector2I& viewportSize);
		~Renderer();

		// 장면을 구성하는 모든 객체(액터)가 화면에 그릴 데이터를 전달하는 함수.
		void SubmitWorld(const PixelImage& image, const Vector2F& position, const Vector2F& pivot = Vector2F::Zero, int sortingOrder = 0, RenderSpace renderSpace = RenderSpace::World);
		
		void SubmitUI(const PixelImage& image, const Vector2F& position, const Vector2F& pivot = Vector2F::Zero, int sortingOrder = 10000, RenderSpace renderSpace = RenderSpace::Screen);
		

		// Draw 이벤트 함수(Engine에서 호출)
		void Draw();

		// 전역 접근 함수.
		static Renderer& Get();

	private:
		// 프레임을 시작할 때 화면을 지우는 함수.
		void Clear();

		// 전달 받은 그리기 명령을 활용해 그리는 함수.
		void DrawRenderQueue();

		// 그린 결과를 화면에 표시하는 함수.
		// 이중 버퍼 구현 시 버퍼를 교환하는 함수.
		void Present();

		// 현재 프레임을 그릴 버퍼 반환 함수.
		const ScreenBuffer* const GetCurrentBuffer() const;


	private:
		// 싱글톤 구현을 위한 전역 변수
		static Renderer* instance;

		// 이번 프레임에 화면에 그릴 데이터를 모아둔 동적 배열(큐).
		std::vector<RenderCommand> renderQueue;

		// 화면 크기.
		Vector2I screenSize;
		Vector2I viewportSize;

		// 글자/그리기 순서 2차원 배열을 관리하는 프레임 객체.
		std::unique_ptr<Frame> frame;

		// 화면 콘솔 버퍼 (이중 버퍼).
		std::unique_ptr<ScreenBuffer> screenBufferArray[2] = {};

		// 백버퍼 인덱스.
		int currentBufferIndex = 0;

	};
}

