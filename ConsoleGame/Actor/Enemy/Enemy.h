#pragma once

#include <Actor/Actor.h>

namespace Craft
{
	class TileMap;
}

class Enemy : public Craft::Actor
{
	TYPE_DECLARATIONS(Enemy, Actor)

	enum class EnemyState
	{
		Patrol,
		Chase,
		Attack,
		Flee,
		Return,
		Dead,
		Count
	};


public:
	Enemy(const Craft::Vector2F& position);
	~Enemy() = default;

	void SetTileMap(const std::shared_ptr<Craft::TileMap>& newMap) { tileMap = newMap; }

	void SetMoveSpeedX(float moveSpeed) { enemyMoveSpeedX = moveSpeed; }
	void SetMoveSpeedY(float moveSpeed) { enemyMoveSpeedY = moveSpeed; }

	// Getter/Setter
	inline float GetHp() const { return Hp; }
	inline bool IsDead() const { return isDead; }

protected:
	virtual void BeginPlay() override;



protected:
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

	// 애니메이션 관련 변수
	float animationElapsedTime = 0.f;
	float animationFrameTime = 0.2f;

	int currentAnimationSpriteIndex = 0;
	int currentStateIndex = 0;

	// 타일 맵 참조
	std::weak_ptr<Craft::TileMap> tileMap;

	// 타겟 참조
	std::weak_ptr<Actor> target;
	
	// 현재 상태
	EnemyState enemyState = EnemyState::Patrol;

	// 적 애니메이션
	std::vector<std::vector<Craft::PixelImage>> enemySpriteAnimation;

};

