#pragma once

#include <Actor/Actor.h>

class Enemy : public Craft::Actor
{
	TYPE_DECLARATIONS(Enemy, Actor)

	enum class EnemyState
	{
		Idle,
		Chase,
		Attack,
	};


public:
	Enemy();
	~Enemy() = default;

	bool SetTarget(const std::shared_ptr<Actor>& target);

	void SetMoveSpeedX(float moveSpeed) { enemyMoveSpeedX = moveSpeed; }
	void SetMoveSpeedY(float moveSpeed) { enemyMoveSpeedY = moveSpeed; }

	// Getter/Setter
	inline float GetHp() const { return Hp; }
	inline bool IsDead() const { return isDead; }



private:
	// 이동속도
	float enemyMoveSpeedX = 0.f;
	float enemyMoveSpeedY = 0.f;
	
	// 체력
	float Hp = 100.f;
	bool isDamaged = false;
	bool isDead = false;

	// 플레이어 감지. 보는 방향
	bool isPlayerFound = false;
	Craft::Vector2F enemySightDir = Craft::Vector2F::Zero;

	// 타겟 참조
	std::weak_ptr<Actor> target;
	
	// 현재 상태
	EnemyState enemyState = EnemyState::Idle;

};

