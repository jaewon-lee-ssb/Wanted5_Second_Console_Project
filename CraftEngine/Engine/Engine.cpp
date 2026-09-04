#include <Engine/Engine.h>
#include <Level/Level.h>
#include <Input/Input.h>
#include <Render/Renderer.h>
#include <Utility/Random.h>
#include <Physics/CollisionSystem.h>
#include <UIManager/UIManager.h>


#include <Windows.h>
#include <stdint.h>
#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>

namespace Craft
{
	// 전역 개체 초기화
	Engine* Engine::instance = nullptr;

	Engine::Engine()
	{
		// 어써트
		assert(!instance && "instance is not null");
		instance = this;

		// 엔진 설정 로드.
		LoadEngineSetting();
		
		// 랜덤 시드 설정.
		Utility::SetRandomSeed();

		// 입력 개체 생성
		input = std::make_unique<Input>();

		// 렌더러 객체 생성.
		renderer = std::make_unique<Renderer>(Vector2I(setting.width, setting.height), Vector2I(setting.viewportwidth, setting.viewportheight));

		// 콜리전 시스템 객체 생성.
		collisionSystem = std::make_unique<CollisionSystem>();

	}

	Engine::~Engine()
	{
		instance = nullptr;
	}

	void Engine::Run()
	{
		// 윈도우즈가 제공하는 고해상도 타이머 (하드웨어 타이머).
		
		// QueryPerformanceFrequency : 타이머의 해상도.
		// 예: 밀리세컨드(1/1000) 해상도 = 1000.
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);

		// 현재 시간 확인
		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter);

		// 프레임 시간 계산을 위한 변수
		int64_t currentTime = counter.QuadPart;
		int64_t previousTime = currentTime;

		// 프레임 고정.
		float oneFrameTime = 1.0f /	setting.framerate;

		while (!isQuit)
		{
			
			// 입력 처리.
			ProcessInput();

			// 프레임 시간 계산
			// 1. 현재 시간 확인.
			QueryPerformanceCounter(&counter);

			// 현재 시간 저장.
			currentTime = counter.QuadPart;

			// 2. 프레임 시간 계산. (현재 시간 - 이전 시간) / 시간 단위 (해상도) 
			//						 -> 초단위로 변환
			float deltaTime = static_cast<float>(currentTime - previousTime) / static_cast<float>(frequency.QuadPart);

			// 고정 프레임 처리.
			// 프레임 사이에 걸린 시간이 목표 시간보다 더 많이 지났으면
			// 프레임 처리
			if (deltaTime >= oneFrameTime)
			{
				
				// 레벨 초기화 이벤트 함수.
				OnInitialized();

				if (mainLevel)
				{
					if (!mainLevel->IsFirstActorAdd())
					{
						mainLevel->ProcessAddAndDestroyActors();
					}
				}

				// 레벨의 액터 초기화 이벤트 함수.
				BeginPlay();

				// 레벨의 액터 업데이트 함수
				Tick(deltaTime);

				// 충돌 처리.
				ProcessCollision();

				// 업데이트된 결과를 화면에 그리는 함수.
				Draw();

				// 레벨 전환 처리.
				if (nextLevel)
				{
					UIManager::Get().Clear();
					// 기존 레벨 정리.
					if (mainLevel)
					{
						mainLevel.reset();
					}

					// 이전 프레임에 전환 요청된 레벨을 메인 레벨로 설정
					mainLevel = std::move(nextLevel);

					// 정리.
					nextLevel.reset();
				}

				// 추가/제거 요청된 액터 정리
				if (mainLevel)
				{
					mainLevel->ProcessAddAndDestroyActors();
					// 액터의 이전 상태 저장.
					mainLevel->SavePreviousActorStates();
				}

				// 처리된 입력을 이전 프레임 입력으로 저장.
				SavePreviousInputStates();

				// 이전 프레임 시간 기록.
				previousTime = currentTime;
			}

		}

		// 종료 처리 함수 호출.
		Shutdown();
	}

	void Engine::Quit()
	{
		// 종료 플래그 설정.
		isQuit = true;
	}

	Engine& Engine::Get()
	{
		// 검증 - 어써트
		// 무조건 필수로 통과해야하는 조건이 있을 때 사용.
		// 디버그 모드에서만 동작
		assert(instance && "instance is null");

		// 전역 개체 반환 (instance는 null이 아니여야함)
		return *instance;
	}

	void Engine::ProcessInput()
	{
		// 인풋 널체크
		assert(input && "input should not be null here");
		if (!input)
		{
			return;
		}

		// 입력 처리 함수 호출
		input->ProcessInput();
	}

	void Engine::OnInitialized()
	{
		// 레벨에 이벤트 전달.
		// 레벨이 이미 초기화 되었으면 처리 안함.
		if (!mainLevel || mainLevel->HasInitialized())
		{
			return;
		}

		mainLevel->OnInitialized();
	}

	void Engine::BeginPlay()
	{
		// 레벨에 이벤트 전달.
		if (!mainLevel)
		{
			return;
		}

		mainLevel->BeginPlay();
	}

	void Engine::Tick(float deltaTime)
	{
		// 레벨에 이벤트 전달.
		if (!mainLevel)
		{
			return;
		}

		mainLevel->Tick(deltaTime);

		UIManager::Get().Tick(deltaTime);
	}

	void Engine::Draw()
	{
		// 레벨에 이벤트 전달.
		if (!mainLevel)
		{
			return;
		}

		mainLevel->Draw();

		UIManager::Get().Draw();
		// 렌더러에 Draw 이벤트 전달(호출).
		renderer->Draw();
	}

	void Engine::ProcessCollision()
	{
		if (!mainLevel)
		{
			return;

		}

		// 레벨의 액터 목록을 충돌 시스템에 전달해 처리 진행.
		collisionSystem->ProcessCollision(mainLevel->actorList);
	}

	void Engine::SavePreviousInputStates()
	{
		// 인풋 널체크
		assert(input && "input should not be null here");
		if (!input)
		{
			return;
		}

		input->SavePreviousStates();
	}

	void Engine::Shutdown()
	{
		UIManager::Get().Clear();
	}

	void Engine::LoadEngineSetting()
	{
		// 엔진 설정 파일 열기.
		std::ifstream file("../Config/Setting.txt");

		// 정상적으로 열렷는지 확인
		assert(file.is_open() && "file not open");

		// 라인(Line) 별로 읽기.
		std::string line;
		while (std::getline(file, line))
		{
			// 빈 줄 및 주석 건너뛰기.
			if (line.empty() || line[0] == '#')
			{
				continue;
			}

			// key = value 포맷 파싱.
			const size_t equalPosition = line.find('=');

			// 라인 문자열에 =문자가 있는지 확인.
			assert(equalPosition != std::string::npos);

			// 좌/우 공백 제거용 람다.
			auto trim = [](std::string& s)
				{
					// 공백 문자 집합. (\r\n -> CRLF) \n(LF).
					// ' ': 스페이스.
					// \t: 탭.
					// \r: 윈도우 개행문자 일부.
					// \n: 개행 문자.
					const char* whiteSpace = " \t\r\n";

					// 문자열의 앞에서부터 공백이 아닌 첫 문자 위치 검색.
					const size_t begin = s.find_first_not_of(whiteSpace);

					// 공백이 아닌 문자를 못찾은 경우에는 빈 문자열로 설정 후 반환.
					if (begin == std::string::npos)
					{
						s.clear();
						return;
					}

					// 문자열의 뒤에서부터 공백이 아닌 마지막 문자 위치 검색.

					const size_t end = s.find_last_not_of(whiteSpace);

					// " abc ". begin=1, end=3, (end-begin)=2
					s = s.substr(begin, end - begin + 1);
				};

			// key 파싱.
			std::string key = line.substr(0, equalPosition);

			// value 파싱.
			std::string value = line.substr(equalPosition + 1);

			// key / value 에서 공백 제거
			trim(key);
			trim(value);

			// key와 value가 제대로 설정됐는지 확인.
			assert(!key.empty() && !value.empty());

			// 속성 읽기.

			// framerate.
			if (key == "framerate")
			{
				setting.framerate = static_cast<float>(atof(value.c_str()));
				assert(setting.framerate > 0.0f);
				continue;
			}

			// width
			if (key == "width")
			{
				setting.width = static_cast<int>(atoi(value.c_str()));
				assert(setting.width > 0);
				continue;
			}

			// height
			if (key == "height")
			{
				setting.height = static_cast<int>(atoi(value.c_str()));
				assert(setting.height > 0);
				continue;
			}

			// viewport width
			if (key == "viewportwidth")
			{
				setting.viewportwidth = static_cast<int>(atoi(value.c_str()));
				assert(setting.viewportwidth > 0);
				continue;
			}

			// viewport height
			if (key == "viewportheight")
			{
				setting.viewportheight = static_cast<int>(atoi(value.c_str()));
				assert(setting.viewportheight > 0);
				continue;
			}
		}

		// 처리가 완료되면 파일 닫기.
		file.close();

	}
}