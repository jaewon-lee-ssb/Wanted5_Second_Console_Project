#pragma once

#include <Actor/Enemy/Enemy.h>

class MiniFish : public Enemy
{
	TYPE_DECLARATIONS(MiniFish, Enemy)

	inline static constexpr float CollisionWidth = 16.f;
	inline static constexpr float CollisionHeight = 8.f;

public:
	MiniFish(const Craft::Vector2F& position);
	~MiniFish() = default;

	static Craft::Bounds GetSpawnBounds(const Craft::Vector2F& position);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

	virtual void UpdateState(float deltaTime) override; 

	void UpdatePatrol(float deltaTime);
	void UpdateFlee(float deltaTime);
	void UpdateReturn(float deltaTime);
	void UpdateDead(float deltaTime);

private:

	


private:
	// 이 거리가 넘으면 도망 종료
	float fleeEndDistance = 70.f;

	// 패트롤 위치
	Craft::Vector2F patrolOrigin;

	// 패트롤 범위
	float patrolRadius = 30.f;
	float patrolWaitTime = 0.f;

	// 다시 패트롤하는 쿨타임 랜덤설정할거임
	float patrolRetryInterval = 0.5f;


	// 이미지 파일이름
	const std::string enemyPatrolFilename = "Actors/Enemy/minifish_patrol.txt";
};

