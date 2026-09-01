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

private:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;


	// 플레이어 감지
	bool DetectPlayer() const;

	// 패트롤 
	void FollowPath(float deltaTime);
	void FindRandomPatrolPoint();

	// 다시 돌아오는 길찾기
	void FindReturnPath();

	// 도망
	void MoveWithTileCollision(const Craft::Vector2F& movement);

	// 초기화
	void ResetPath();

	// 행동 업데이트
	void UpdatePatrol(float deltaTime);
	void UpdateFlee(float deltaTime);
	void UpdateReturn(float deltaTime);

private:
	// 이 거리가 넘으면 도망 종료
	float fleeEndDistance = 70.f;

	// 패트롤 위치
	Craft::Vector2F patrolOrigin;
	Craft::Vector2F patrolTarget;

	// 패트롤 범위
	float patrolRadius = 30.f;
	float patrolWaitTime = 0.f;

	// 다시 패트롤하는 쿨타임 랜덤설정할거임
	float patrolRetryInterval = 0.5f;

	bool isFindTarget = false;

	// 감지 범위
	float detectRadius = 50.f;

	// A* 관련 변수
	std::vector<Craft::Vector2I> patrolPath;
	size_t currentPathIndex = 0;

	// 이미지 파일이름
	const std::string enemyPatrolFilename = "Actors/Enemy/minifish_patrol.txt";
};

