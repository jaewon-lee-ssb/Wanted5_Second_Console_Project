#pragma once

#include <Actor/Actor.h>

#include <Actor/Enemy/EnemyTypes.h>

namespace Craft
{
	class TileMap;
}

class Enemy : public Craft::Actor
{
	TYPE_DECLARATIONS(Enemy, Actor)

public:
	Enemy(const Craft::Vector2F& position);
	~Enemy() = default;

	void SetTileMap(const std::shared_ptr<Craft::TileMap>& newMap) { tileMap = newMap; }

	void SetMoveSpeed(float moveSpeed) { enemyMoveSpeed = moveSpeed; }

	// Getter/Setter
	inline float GetHp() const { return Hp; }
	inline bool IsDead() const { return isDead; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

	// 현재 상태따라 업데이트 상태 불러줌
	virtual void UpdateState(float deltaTime);

	// 애니메이션 업데이트 함수
	void UpdateAnimation(float deltaTime);

	// 상태 변화 함수
	void ChangeEnemyState(const EnemyState& state) { enemyState = state; }

	// 타겟과의 거리
	bool DetectTarget() const;

	// 패트롤 
	void FollowPath(float deltaTime);
	void FindRandomPatrolPoint(const float& patrolRadius);

	// 다시 돌아오는 길찾기
	void FindReturnPath();

	// 도망
	void MoveWithTileCollision(const Craft::Vector2F& movement);

	// 초기화
	void ResetPath();


protected:
	// 이동속도
	float enemyMoveSpeed = 0.f;
	
	// 체력
	float Hp = 100.f;
	bool isDamaged = false;
	bool isDead = false;

	// 감지 범위
	float detectRadius = 50.f;

	// 패트롤 위치
	Craft::Vector2F patrolOrigin;

	// 패트롤 범위
	//float patrolRadius = 30.f;

	// 애니메이션 관련 변수
	float animationElapsedTime = 0.f;
	float animationFrameTime = 0.2f;

	int currentAnimationSpriteIndex = 0;
	int currentStateIndex = 0;

	// A* 관련 변수
	std::vector<Craft::Vector2I> patrolPath;
	size_t currentPathIndex = 0;

	// 타일 맵 참조
	std::weak_ptr<Craft::TileMap> tileMap;

	// 타겟 참조
	std::weak_ptr<Actor> targetPtr;
	
	// 현재 상태
	EnemyState enemyState = EnemyState::Patrol;

	// 적 애니메이션
	std::vector<std::vector<Craft::PixelImage>> enemySpriteAnimation;

};

