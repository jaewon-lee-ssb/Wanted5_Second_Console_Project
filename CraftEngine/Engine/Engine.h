#pragma once

#include <Core/Core.h>

#include <memory>
#include <string>
#include <type_traits>
#include <utility>

class Sound;

namespace Craft
{
	// 전방 선언 (컴파일 시간 단축).
	class Level;
	class Input;
	class Renderer;
	class CollisionSystem;
	class UIManager;
	

	// 메인 엔진 클래스.
	// 엔진 루프를 제공.
	// 게임 엔진의 핵심 기능 제공.
	class CRAFT_API Engine
	{
		// 엔진 설정 구조체 (데이터)
		struct EngineSetting
		{
			// 타겟 프레임 속도
			float framerate = 0.0f;

			// 화면 가로 크기.
			int width = 0;

			// 화면 세로 크기.
			int height = 0;

			// 게임화면 가로 크기.
			int viewportwidth = 0;

			// 게임화면 세로 크기.
			int viewportheight = 0;
		};

	public:
		Engine();
		virtual ~Engine();

		// 게임 루프 실행 함수
		void Run();

		// 엔진 종료 함수
		void Quit();

		// 사운드 재생 함수(사운드 시스템 래퍼 함수)
		void PlayOneShot(const std::string& filename);
		void PlayBackgroundMusic(const std::string& filename);
		void StopBackgroundMusic();

		// 레벨 추가 요청 함수.
		// TODO: 나중에 밑에 것들 찾아보기
		// 1. std::is_base_of 하는일이 무엇인지
		// 2. std::enable_if_t 하는일이 무엇인지
		// 3. typename = std::enable_if_t<std::is_base_of<Level, T>::value>>
		template<typename T, typename ...Args, typename = std::enable_if_t<std::is_base_of<Level, T>::value>>
		void AddNewLevel(Args&& ...args)
		{
			// 추가 요청 레벨 객체 생성.
			nextLevel = std::make_shared<T>(std::forward<Args>(args)...);
		}

		// 싱글톤 접근 함수.
		static Engine& Get();

		// Getter.
		inline int GetWidth() const { return setting.width; }
		inline int GetHeight() const { return setting.height; }
		inline int GetViewportWidth() const { return setting.viewportwidth; }
		inline int GetViewportHeight() const { return setting.viewportheight; }

	protected:
		// 입력 처리 함수. (입력 폴링).
		void ProcessInput();
		
		// 초기화 함수.
		// 레벨 초기화 함수.
		void OnInitialized();

		// 게임 플레이 이벤트 함수.
		// 액터 초기화 함수.
		void BeginPlay();

		// 업데이트 함수.
		void Tick(float deltaTime);

		// 화면에 그리는 함수.
		void Draw();

		// 충돌 처리 함수.
		void ProcessCollision();

		// 이전 입력을 저장하는 함수.
		void SavePreviousInputStates();

		// 엔진 종료 시 정리 함수.
		void Shutdown();

		// 엔진 설정 로드 함수.
		void LoadEngineSetting();


	protected:
		// 엔진 설정.
		EngineSetting setting;

		// 엔진 종료 플래그
		bool isQuit = false;

		// 싱글톤을 위한 전역 개체
		static Engine* instance;

		// 메인 레벨.
		std::shared_ptr<Level> mainLevel;

		// 추가 요청된 레벨.
		std::shared_ptr<Level> nextLevel;

		// 입력 시스템
		std::unique_ptr<Input> input;

		//렌더러.
		std::unique_ptr<Renderer> renderer;


		// 충돌 시스템 객체
		std::unique_ptr<CollisionSystem> collisionSystem;

		std::unique_ptr<Sound> sound;
	};
}

