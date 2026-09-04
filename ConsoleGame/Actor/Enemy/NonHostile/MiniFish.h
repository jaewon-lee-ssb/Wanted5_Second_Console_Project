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
	void UpdateDamaged(float deltaTime);
	void UpdateReturn(float deltaTime);
	void UpdateDead(float deltaTime);

	

private:
	virtual void InitEnemy() override;

private:

	// 이미지 파일이름
	const std::string enemyPatrolFilename = "Actors/Enemy/minifish_patrol.txt";
	const std::string enemyDamagedFilename = "Actors/Enemy/minifish_damaged.txt";
	const std::string enemyFleeFilename = "Actors/Enemy/minifish_flee.txt";
	const std::string enemyDeadFilename = "Actors/Enemy/minifish_dead.txt";

};

