#pragma once

#include <Core/Core.h>

#include <random>

namespace Utility
{
	// 랜덤 성능 지표: 무작위 수를 얼마나 잘 생성해주는가/속도.

	inline CRAFT_API std::mt19937& GetRandomEngine()
	{
		// 프로그램 실행 중에 1번만 생성.
		static std::mt19937 engine;

		// 랜덤 엔진 반환.
		return engine;
	}
	
	// 랜덤 시드(종잣 값) 설정 함수.
	inline CRAFT_API void SetRandomSeed()
	{
		// 하드웨어 기반 난수 생성기 (시드값 제공용).
		std::random_device randomDevice;

		// 랜덤 엔진에 종자값 설정.
		GetRandomEngine().seed(randomDevice());
	}

	// 정수 난수 함수.
	inline int RandomRange(int min, int max)
	{
		// min에서 max까지 균등하게 정수 난수를 생성해주는 분포 정의.
		std::uniform_int_distribution<int> distribution(min, max);
		
		// 랜덤 엔진을 통해 무작의 수 반환.
		return distribution(GetRandomEngine());
	}

	// 부동소수점 난수 함수.
	inline float RandomRange(float min, float max)
	{
		// min에서 max까지 균등하게 정수 난수를 생성해주는 분포 정의.
		std::uniform_real_distribution<float> distribution(min, max);

		// 랜덤 엔진을 통해 무작의 수 반환.
		return distribution(GetRandomEngine());
	}
}