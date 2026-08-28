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

	void FollowPath(float deltaTime);

private:
	// 패트롤 위치
	Craft::Vector2F patrolOrigin;
	Craft::Vector2F patrolTarget;

	// 패트롤 범위
	float patrolRadiusX = 100.f;
	float patrolRadiusY = 30.f;
	float patrolWaitTime = 0.f;

	// A* 관련 변수
	std::vector<Craft::Vector2I> patrolPath;
	size_t currentPathIndex = 0;

	// 이미지 파일이름
	const std::string enemyPatrolFilename = "Actors/Enemy/minifish_patrol.txt";
};

