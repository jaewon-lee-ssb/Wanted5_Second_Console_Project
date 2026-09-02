#pragma once

#include <Actor/Enemy/Enemy.h>

class Pufferfish : public Enemy
{
	TYPE_DECLARATIONS(Pufferfish, Enemy)

	inline static constexpr float CollisionWidth = 16.f;
	inline static constexpr float CollisionHeight = 8.f;

public:
	Pufferfish(const Craft::Vector2F& position);
	~Pufferfish() = default;

	static Craft::Bounds GetSpawnBounds(const Craft::Vector2F& position);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	
	virtual void UpdateState(float deltaTime) override;

	void UpdatePatrol(float deltaTime) ;
	void UpdateChase(float deltaTime) ;
	void UpdateAttack(float deltaTime) ;
	void UpdateFlee(float deltaTime) ;
	void UpdateReturn(float deltaTime) ;
	void UpdateDead(float deltaTime) ;

private:

	// 패트롤 위치
	Craft::Vector2F patrolOrigin;
	Craft::Vector2F patrolTarget;

	// 패트롤 범위
	float patrolRadius = 30.f;
	float patrolWaitTime = 0.f;

	// 다시 패트롤하는 쿨타임 랜덤설정할거임
	float patrolRetryInterval = 0.5f;

	// 감지 범위
	float detectRadius = 50.f;

	// A* 관련 변수
	std::vector<Craft::Vector2I> patrolPath;
	size_t currentPathIndex = 0;

	// 이미지 파일이름
	const std::string enemyPatrolFilename = "Actors/Enemy/pufferfish_attack.txt";
};

