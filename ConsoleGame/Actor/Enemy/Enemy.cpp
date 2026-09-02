#include <Actor/Enemy/Enemy.h>
#include <Actor/Player/Player.h>

#include <Level/Level.h>
#include <World/TileMap.h>
#include <Utility/Random.h>

Enemy::Enemy(const Craft::Vector2F& position)
	: super({}, position)
{
	enemySpriteAnimation.resize(static_cast<int>(EnemyState::Count));
}

void Enemy::BeginPlay()
{
	targetPtr = GetOwner()->FindActor<Player>();
}

void Enemy::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	UpdateState(deltaTime);

	UpdateAnimation(deltaTime);
}

void Enemy::UpdateState(float deltaTime)
{
	
}

void Enemy::UpdateAnimation(float deltaTime)
{
	// 애니메이션 시간 추가
	animationElapsedTime += deltaTime;


	// 애니메이션 프레임 교체
	if (!enemySpriteAnimation[currentStateIndex].empty() && animationElapsedTime >= animationFrameTime)
	{
		animationElapsedTime -= animationFrameTime;

		currentAnimationSpriteIndex = (currentAnimationSpriteIndex + 1) % enemySpriteAnimation[currentStateIndex].size();

		ChangeImage(enemySpriteAnimation[currentStateIndex][currentAnimationSpriteIndex]);
	}
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

void Enemy::FollowPath(float deltaTime)
{
	auto map = tileMap.lock();

	if (!map)
	{
		return;
	}

	// 목표 지점에 도착하면 초기화
	if (currentPathIndex >= patrolPath.size())
	{
		ResetPath();
		return;
	}

	// A*가 반환한 현재 타일 좌표
	const Craft::Vector2I pathTile = patrolPath[currentPathIndex];

	// 타일의 왼쪽 위 월드 좌표
	Craft::Vector2F targetPosition = map->TileToWorld(pathTile.x, pathTile.y);

	// 타일 중앙 좌표로 보정
	const Craft::Vector2I tileSize = map->GetTileSize();

	targetPosition = targetPosition + Craft::Vector2F(tileSize.x * 0.5f, tileSize.y * 0.5f);

	// 여기부터 목표 좌표를 향해 이동
	Craft::Vector2F difference = targetPosition - GetPosition();
	const Craft::Vector2F direction = difference.Normalize();

	// 거리 비교를 위해계산을 위한 제곱
	const float adjustedDistanceSquared = Craft::GetDistanceSquared(difference);

	const Craft::Vector2F movement(direction.x * enemyMoveSpeed * 2 * deltaTime, direction.y * enemyMoveSpeed * deltaTime);

	const float adjustedMovementSquared = Craft::GetDistanceSquared(movement);

	if (adjustedDistanceSquared <= 0.5f * 0.5f || adjustedMovementSquared >= adjustedDistanceSquared)
	{
		if (!map->OverlapsSolid(GetBoundsAt(targetPosition)))
		{
			SetPosition(targetPosition);
			++currentPathIndex;
		}

		return;
	}

	const Craft::Vector2F newPosition = GetPosition() + movement;

	if (!map->OverlapsSolid(GetBoundsAt(newPosition)))
	{
		SetPosition(newPosition);
	}
	else
	{
		ResetPath();
	}
}

void Enemy::FindRandomPatrolPoint(const float& patrolRadius)
{
	// 패트롤 시작 지점으로 부터 범위내 랜덤 위치로 패트롤 길 찾기
	const float randomTargetX = Utility::RandomRange(-patrolRadius * 2, patrolRadius * 2);
	const float randomTargetY = Utility::RandomRange(-patrolRadius, patrolRadius);

	if (auto map = tileMap.lock())
	{
		patrolPath = map->FindPath(GetPosition(), Craft::Vector2F(patrolOrigin.x + randomTargetX, patrolOrigin.y + randomTargetY), static_cast<float>(GetWidth()), static_cast<float>(GetHeight()));
	}
}

void Enemy::FindReturnPath()
{
	// 패트롤 시작지점으로 돌아가는 길 찾기
	if (auto map = tileMap.lock())
	{
		patrolPath = map->FindPath(GetPosition(), patrolOrigin, static_cast<float>(GetWidth()), static_cast<float>(GetHeight()));
	}
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

	if (!map->OverlapsSolid(GetBoundsAt(newPosition)))
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

	if (!map->OverlapsSolid(GetBoundsAt(newPosition)))
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
	patrolPath.clear();
	currentPathIndex = 0;
}
