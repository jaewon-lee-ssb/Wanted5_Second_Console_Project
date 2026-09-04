#pragma once

#include <Actor/Actor.h>

#include <Actor/Enemy/EnemyTypes.h>

#include <Utility/Timer.h>

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

	// 타일맵 지정함수
	void SetTileMap(const std::shared_ptr<Craft::TileMap>& newMap) { tileMap = newMap; }

	void SetMoveSpeed(float moveSpeed) { enemyMoveSpeed = moveSpeed; }

	void TakeDamage(float damage);

	// Getter/Setter
	inline bool IsDead() const { return isDead; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void OnCollision(const std::shared_ptr<Craft::Actor>& other) override;

	// 현재 상태따라 업데이트 상태 불러줌
	virtual void UpdateState(float deltaTime);

	// 애니메이션 업데이트 함수
	void UpdateAnimation(float deltaTime);

	// 상태 변화 함수
	void ChangeEnemyState(const EnemyState& state) { curState = state; }

	// 타겟과의 거리
	bool DetectTarget() const;

	bool CheckDead();

	// 길 따라가는 함수
	bool FollowPath(float deltaTime);

	// 길 찾기 함수
	bool FindPathTo(const Craft::Vector2F& destination);

	// 충돌체크하면서 이동하는 함수
	void MoveWithTileCollision(const Craft::Vector2F& movement);

	bool FindRandomPatrolPoint(Craft::Vector2F& randPosition, const float& patrolRadius);

	// 다시 돌아오는 길찾기
	bool FindReturnPath();

	// 초기화
	void ResetPath();

	// 부모 변수 초기화
	virtual void InitEnemy();


protected:
	// 이동속도
	float enemyMoveSpeed = 0.f;
	
	// 체력
	float Hp = 0;
	bool isDamaged = false;
	bool isDead = false;

	// 감지 범위
	float detectRadius = 0.f;

	// 패트롤 범위
	float patrolRadius = 0.f;

	// 다시 패트롤하는 쿨타임 랜덤설정할거임
	float patrolRetryInterval = 0.f;

	// 이 거리가 넘으면 도망 종료
	float fleeEndDistance = 0.f;

	// 패트롤 위치
	Craft::Vector2F patrolOrigin;

	// 애니메이션 관련 변수
	float animationFrameTime = 0.2f;

	// 각종 타이머
	Utility::Timer animationTimer;
	Utility::Timer enemyWaitTimer;

	// 현재 몇번째 프레임 가리키는 인덱스
	int currentAnimationSpriteIndex = 0;

	// A* 관련 변수
	std::vector<Craft::Vector2I> movePath;
	size_t currentPathIndex = 0;

	// 타일 맵 참조
	std::weak_ptr<Craft::TileMap> tileMap;

	// 타겟 참조
	std::weak_ptr<Actor> targetPtr;
	
	// 현재 상태
	EnemyState curState = EnemyState::Patrol;

	// 적 애니메이션
	std::vector<std::vector<Craft::PixelImage>> enemySpriteAnimation;

};

