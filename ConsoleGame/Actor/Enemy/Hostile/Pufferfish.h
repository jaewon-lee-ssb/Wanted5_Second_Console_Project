#pragma once

#include <Actor/Enemy/Enemy.h>

class Pufferfish : public Enemy
{
	TYPE_DECLARATIONS(Pufferfish, Enemy)

	inline static constexpr float CollisionWidth = 24.f;
	inline static constexpr float CollisionHeight = 12.f;

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
	virtual void InitEnemy() override;

private:
	Utility::Timer chaseTimer;

	int findTargetCount = 0;

	// 이미지 파일이름
	const std::string enemyPatrolFilename = "Actors/Enemy/pufferfish_attack.txt";
};

