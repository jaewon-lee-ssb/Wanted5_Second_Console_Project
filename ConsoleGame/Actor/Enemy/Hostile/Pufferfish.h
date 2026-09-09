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
	void UpdateDamaged(float deltaTime);
	void UpdateFlee(float deltaTime) ;
	void UpdateReturn(float deltaTime) ;
	void UpdateDead(float deltaTime) ;

private:
	virtual void InitEnemy() override;

	bool AttackRangeCheck();

private:
	Utility::Timer chaseTimer;
	Utility::Timer attackCooltime;
	Utility::Timer lifetimeTimer;

	int findTargetCount = 0;

	float attackRadius = 0.f;

	float attackDamage = 10.f;

	bool isAttackReady = false;

	// 이미지 파일이름
	const std::string enemyPatrolFilename = "Actors/Enemy/pufferfish_patrol.txt";
	const std::string enemyChaseFilename = "Actors/Enemy/pufferfish_chase.txt";
	const std::string enemyAttackFilename = "Actors/Enemy/pufferfish_attack.txt";
	const std::string enemyDamagedFilename = "Actors/Enemy/pufferfish_damaged.txt";
	const std::string enemyFleeFilename = "Actors/Enemy/pufferfish_flee.txt";
	const std::string enemyDeadFilename = "Actors/Enemy/pufferfish_dead.txt";
};

