#pragma once

#include <Core/Core.h>

namespace Utility
{
	class CRAFT_API Timer
	{
	public:
		Timer(float targetTime = 1.f);

		// 타이머 업데이트 함수.
		void Tick(float deltaTime);

		// 경과 시간 리셋 함수.
		void Reset();

		void Start();

		void Stop();

		// 목표 시간 재설정 함수.
		void SetTargetTime(float newTargetTime);

		// 설정한 시간만큼 흘렀는지 확인하는 함수.
		inline bool IsTimeOut() const { return elapsedTime >= targetTime; }

		// 0~1 범위의 타이머 진행률을 반환한다.
		inline float GetProgress() const
		{
			if (targetTime <= 0.f || elapsedTime >= targetTime)
			{
				return 1.f;
			}

			return elapsedTime / targetTime;
		}

	private:
		// 경과 시간 계산용 변수.
		float elapsedTime = 0.f;

		// 타이머 목표 시간.
		float targetTime = 0.f;

		bool isStop = false;
	};
}

