#pragma once

#include <Actor/Enemy/Enemy.h>

class MiniFish : public Enemy
{
	TYPE_DECLARATIONS(MiniFish, Enemy)

public:
	MiniFish(const Craft::Vector2F& position);
	~MiniFish() = default;


private:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;


	// 플레이어 감지
	void DetectPlayer();

	// 패트롤 
	void FollowPath(float deltaTime);
	void FindRandomPatrolPoint();

	// 

	// 행동 업데이트
	void UpdatePatrol(float deltaTime);
	void UpdateFlee(float deltaTime);
	void UpdateReturn(float deltaTime);

private:
	// 적의 현재 상태

	// 패트롤 위치
	Craft::Vector2F patrolOrigin;
	Craft::Vector2F patrolTarget;

	// 패트롤 범위
	float patrolRadiusX = 100.f;
	float patrolRadiusY = 30.f;
	float patrolWaitTime = 0.f;
	float patrolRetryInterval = 0.5f;

	// 감지 범위
	float detectRadius = 25.f;

	// A* 관련 변수
	std::vector<Craft::Vector2I> patrolPath;
	size_t currentPathIndex = 0;

	// 이미지 파일이름
	const std::string enemyPatrolFilename = "Actors/Enemy/minifish_patrol.txt";
};

