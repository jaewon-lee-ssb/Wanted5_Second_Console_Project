#include <Actor/Enemy/Enemy.h>
#include <Actor/Player/Player.h>
#include <Collision/GameCollisionLayers.h>

#include <Level/Level.h>
#include <World/TileMap.h>
#include <Utility/Random.h>

#include <cmath>
#include <algorithm>

Enemy::Enemy(const Craft::Vector2F& position)
	: super({}, position), patrolOrigin(position)
{
	enemySpriteAnimation.resize(static_cast<int>(EnemyState::Count));

	animationTimer.SetTargetTime(animationFrameTime);

	SetCollisionLayer(GameCollision::Enemy);
	SetCollisionMask(GameCollision::PlayerAttack);

	InitEnemy();
}

void Enemy::TakeDamage(float damage)
{
	Hp -= damage;

	ChangeEnemyState(EnemyState::Damaged);
	enemyWaitTimer.SetTargetTime(0.2f * enemySpriteAnimation[static_cast<int>(curState)].size());
	enemyWaitTimer.Reset();

	if (Hp <= 0.f)
	{
		Hp = 0.f;
		ChangeEnemyState(EnemyState::Dead);
		enemyWaitTimer.SetTargetTime(0.2f * enemySpriteAnimation[static_cast<int>(curState)].size());
		enemyWaitTimer.Reset();
	}
}

void Enemy::BeginPlay()
{
	targetPtr = GetOwner()->FindActor<Player>();
}

void Enemy::Tick(float deltaTime)
{
	super::Tick(deltaTime);
	float newDeltaTime = deltaTime;

	if (auto target = targetPtr.lock())
	{
		if (auto player = Cast<Player>(target))
		{
			newDeltaTime = deltaTime * (player->GetIsAiming() ? 0.25f : 1.f);
		}
		
	}

	UpdateState(newDeltaTime);

	UpdateAnimation(newDeltaTime);
}

void Enemy::OnCollision(const std::shared_ptr<Craft::Actor>& other)
{
	super::OnCollision(other);

	if (!other)
	{
		return;
	}

	if ((other->GetCollisionLayer() & GameCollision::PlayerAttack) == 0)
	{
		return;
	}

	// 충돌체크
	
	isDamaged = true;
}

void Enemy::UpdateState(float deltaTime)
{
	enemyWaitTimer.Tick(deltaTime);
}

void Enemy::UpdateAnimation(float deltaTime)
{
	// 애니메이션 시간 추가
	animationTimer.Tick(deltaTime);

	// 애니메이션 프레임 교체
	if (!enemySpriteAnimation[static_cast<int>(curState)].empty() && animationTimer.IsTimeOut())
	{
		currentAnimationSpriteIndex = (currentAnimationSpriteIndex + 1) % enemySpriteAnimation[static_cast<int>(curState)].size();

		ChangeImage(enemySpriteAnimation[static_cast<int>(curState)][currentAnimationSpriteIndex]);
		
		animationTimer.Reset();
	}
}

void Enemy::ChangeEnemyState(const EnemyState& state)
{
	if (curState == state)
	{
		return;
	}

	curState = state;
	ChangeImage(enemySpriteAnimation[static_cast<int>(curState)][0]);
	currentAnimationSpriteIndex = 0;
	animationTimer.Reset();
}

bool Enemy::DetectTarget() const
{
	if (auto target = targetPtr.lock())
	{
		const Craft::Vector2F difference = target->GetPosition() - GetPosition();

		// 타겟과의 차이 거리 제곱
		const float adjustedDistanceSquared = Craft::GetDistanceSquared(difference);
		
		// 감지 범위의 길이 제곱
		const float detectDistanceSquared = detectRadius * detectRadius;

		if (adjustedDistanceSquared <= detectDistanceSquared)
		{
			return true;
		}
	}
	return false;
}

bool Enemy::CheckDead()
{
	if (Hp <= 0.f)
	{
		ChangeEnemyState(EnemyState::Dead);
		isDead = true;
		return true;
	}
	return false;
}

bool Enemy::FollowPath(float deltaTime)
{
	auto map = tileMap.lock();

	if (!map)
	{
		return false;
	}

	// 목표 지점에 도착하면 초기화
	if (currentPathIndex >= movePath.size())
	{
		ResetPath();
		return true;
	}

	// A*가 반환한 현재 타일 좌표
	const Craft::Vector2I pathTile = movePath[currentPathIndex];

	// 타일의 왼쪽 위 월드 좌표
	Craft::Vector2F targetPosition = map->TileToWorld(pathTile.x, pathTile.y);

	// 타일 중앙 좌표로 보정
	const Craft::Vector2I tileSize = map->GetTileSize();

	targetPosition = targetPosition + Craft::Vector2F(tileSize.x * 0.5f, tileSize.y * 0.5f);

	// 여기부터 목표 좌표를 향해 이동
	Craft::Vector2F difference = targetPosition - GetPosition();

	if (difference == 0.f)
	{
		// 이미 도착했다면 반환
		++currentPathIndex;

		return true;
	}


	const Craft::Vector2F direction = difference.Normalize();

	flipX = direction.x > 0 ? true : false;

	// 거리 비교를 위해계산을 위한 제곱
	const float adjustedDistanceSquared = Craft::GetDistanceSquared(difference);

	const float distance = std::sqrt(adjustedDistanceSquared);

	const float ratio = (std::min)(1.f, enemyMoveSpeed * 2.f * deltaTime / distance);
	const Craft::Vector2F movement = difference * ratio;

	const float adjustedMovementSquared = Craft::GetDistanceSquared(movement);

	if (adjustedDistanceSquared <= 0.5f * 0.5f || adjustedMovementSquared >= adjustedDistanceSquared)
	{
		if (map->CanOccupyWorld(GetBoundsAt(targetPosition)))
		{
			SetPosition(targetPosition);
			++currentPathIndex;
		}

		return true;
	}

	const Craft::Vector2F newPosition = GetPosition() + movement;

	if (map->CanOccupyWorld(GetBoundsAt(newPosition)))
	{
		SetPosition(newPosition);
	}
	else
	{
		ResetPath();
	}
	return false;
}

bool Enemy::FindPathTo(const Craft::Vector2F& destination)
{
	// 새 경로를 받으므로 이전 경로 초기화
	ResetPath();

	auto map = tileMap.lock();

	if (!map)
	{
		// 타일맵이 존재 하지 않는다면 길찾기 실패
		return false;
	}

	// 목적지에 들어갈 수 없다면 길찾기 실패
	if (!map->CanOccupyWorld(GetBoundsAt(destination)))
	{
		return false;
	}

	movePath = map->FindPath(GetPosition(), destination, static_cast<float>(GetWidth()), static_cast<float>(GetHeight()));

	return !movePath.empty();
}

bool Enemy::FindRandomPatrolPoint(Craft::Vector2F& position, const float& patrolRadius)
{
	auto map = tileMap.lock();
	if (!map)
	{
		return false;
	}
	
	Craft::Vector2F newRandomPosition = Craft::Vector2F::Zero;

	newRandomPosition.x = Utility::RandomRange(-patrolRadius * 2, patrolRadius * 2);
	newRandomPosition.y = Utility::RandomRange(-patrolRadius, patrolRadius);
	
	// 새로운 랜덤 좌표에 액터가 들어갈수 없으면 반환
	if (!map->CanOccupyWorld(GetBoundsAt(position + newRandomPosition)))
	{	
		return false;
	}

	// 들어갈 수 있다면 새로운 위치로 갱신
	position = position + newRandomPosition;
	return true;
}

bool Enemy::FindReturnPath()
{
	// 패트롤 시작지점으로 돌아가는 길 찾기

	return FindPathTo(patrolOrigin);
}

void Enemy::MoveWithTileCollision(const Craft::Vector2F& movement)
{
	auto map = tileMap.lock();

	if (!map)
	{
		return;
	}

	Craft::Vector2F newPosition = GetPosition();

	// X축 이동
	newPosition.x += movement.x;

	if (map->CanOccupyWorld(GetBoundsAt(newPosition)))
	{
		// 갈수 있는 곳이면 이동
		SetPosition(newPosition);
	}
	else
	{
		// 갈수 없으면 이동 안함
		newPosition.x = GetPosition().x;
	}

	// Y축 이동
	newPosition.y += movement.y;

	if (map->CanOccupyWorld(GetBoundsAt(newPosition)))
	{
		// 갈수 있으면 이동
		SetPosition(newPosition);
	}
	else
	{
		// 갈수 없으면 이동 안함
		newPosition.y = GetPosition().y;
	}
}


void Enemy::ResetPath()
{
	// 길 저장해있는거 초기화
	movePath.clear();
	currentPathIndex = 0;
	enemyWaitTimer.Reset();
}

void Enemy::InitEnemy()
{
	
}
