#include <Actor/Enemy/NonHostile/MiniFish.h>

#include <Resource/TextImageLoader.h>
#include <Utility/Random.h>

#include <World/TileMap.h>

MiniFish::MiniFish(const Craft::Vector2F& position)
	: super(position)
{
	enemyState = EnemyState::Patrol;
	enemySpriteAnimation[static_cast<int>(EnemyState::Patrol)] = Craft::TextImageLoader::LoadAnimation(enemyPatrolFilename);

	currentStateIndex = static_cast<int>(EnemyState::Patrol);
	ChangeImage(enemySpriteAnimation[currentStateIndex][0]);


	// 속도 지정
	enemyMoveSpeed = 30.f;

	patrolOrigin = position;
	 
	// 패트롤 지연타임
	patrolRetryInterval = Utility::RandomRange(0.f, 0.5f);
}

Craft::Bounds MiniFish::GetSpawnBounds(const Craft::Vector2F& position)
{
	const float halfWidth = CollisionWidth * 0.5f;
	const float halfHeight = CollisionHeight * 0.5f;

	return Craft::Bounds(position.x - halfWidth, position.x + halfWidth, position.y - halfHeight, position.y + halfHeight);
}

void MiniFish::BeginPlay()
{
	super::BeginPlay();


}

void MiniFish::Tick(float deltaTime)
{
	super::Tick(deltaTime);
	
	// 순찰중이거나 다시 돌아오는중에 플레이어를 찾으면 다시 도망
	if ((enemyState == EnemyState::Patrol || enemyState == EnemyState::Return) && DetectTarget())
	{
		ChangeEnemyState(EnemyState::Flee);

		ResetPath();
	}

	
	// 디버그 경로 출력
	if (auto map = tileMap.lock())
	{
		map->QueueDebugPath(patrolPath, currentPathIndex);
	}

	
}

void MiniFish::FollowPath(float deltaTime)
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

	const float distance = difference.Length();

	const Craft::Vector2F direction = difference.Normalize();

	const Craft::Vector2F movement(direction.x * enemyMoveSpeed * 2 * deltaTime, direction.y * enemyMoveSpeed * deltaTime);

	if (distance <= 0.5f || movement.Length() >= distance)
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

void MiniFish::FindRandomPatrolPoint()
{
	const float randomTargetX = Utility::RandomRange(-patrolRadius * 2, patrolRadius * 2);
	const float randomTargetY = Utility::RandomRange(-patrolRadius, patrolRadius);

	if (auto map = tileMap.lock())
	{
		patrolPath = map->FindPath(GetPosition(), Craft::Vector2F(patrolOrigin.x + randomTargetX, patrolOrigin.y + randomTargetY), static_cast<float>(GetWidth()), static_cast<float>(GetHeight()));
	}
}

void MiniFish::FindReturnPath()
{
	if (auto map = tileMap.lock())
	{
		patrolPath = map->FindPath(GetPosition(), patrolOrigin, static_cast<float>(GetWidth()), static_cast<float>(GetHeight()));
	}
}

void MiniFish::MoveWithTileCollision(const Craft::Vector2F& movement)
{
	auto map = tileMap.lock();

	if (!map)
	{
		return;
	}

	Craft::Vector2F newPosition = GetPosition();

	// X축 이동
	newPosition.x += movement.x;

	if (!map->OverlapsSolid(
		GetBoundsAt(newPosition)))
	{
		SetPosition(newPosition);
	}
	else
	{
		newPosition.x = GetPosition().x;
	}

	// Y축 이동
	newPosition.y += movement.y;

	if (!map->OverlapsSolid(
		GetBoundsAt(newPosition)))
	{
		SetPosition(newPosition);
	}
}

void MiniFish::ResetPath()
{
	patrolPath.clear();
	currentPathIndex = 0;
}

void MiniFish::UpdatePatrol(float deltaTime)
{
	super::UpdatePatrol(deltaTime);

	if (patrolPath.empty())
	{
		patrolWaitTime -= deltaTime;

		if (patrolWaitTime <= 0.f)
		{
			FindRandomPatrolPoint();
			patrolRetryInterval = Utility::RandomRange(0.f, 0.5f);
			patrolWaitTime = patrolRetryInterval;
		}
	}
	else
	{
		FollowPath(deltaTime);
	}
}

void MiniFish::UpdateFlee(float deltaTime)
{
	super::UpdateFlee(deltaTime);

	auto player = targetPtr.lock();
	auto map = tileMap.lock();

	if (!player || !map)
	{
		return;
	}

	const Craft::Vector2F difference = GetPosition() - player->GetPosition();

	const float adjustedDistanceSquared = Craft::GetDistanceSquared(difference);
	const float fleeDistanceSquared = fleeEndDistance * fleeEndDistance;

	if (adjustedDistanceSquared >= fleeDistanceSquared)
	{
		ChangeEnemyState(EnemyState::Return);

		ResetPath();

		return;
	}

	if (adjustedDistanceSquared <= 0.f)
	{
		return;
	}

	const Craft::Vector2F direction = difference.Normalize();

	const Craft::Vector2F movement{ direction.x * enemyMoveSpeed * 2 * deltaTime, direction.y * enemyMoveSpeed * deltaTime };

	MoveWithTileCollision(movement);
}

void MiniFish::UpdateReturn(float deltaTime)
{
	super::UpdateReturn(deltaTime);

	if (patrolPath.empty())
	{
		patrolWaitTime -= deltaTime;

		if (patrolWaitTime <= 0.f)
		{
			if (auto map = tileMap.lock())
			{
				patrolPath = map->FindPath(GetPosition(), patrolOrigin, static_cast<float>(GetWidth()), static_cast<float>(GetHeight()));
			}
			patrolRetryInterval = Utility::RandomRange(0.f, 0.5f);
			patrolWaitTime = patrolRetryInterval;
		}
	}
	else
	{
		FollowPath(deltaTime);
	}
	

	// 어느정도 거리안에 들어왔으면 다시 패트롤 시작

	const Craft::Vector2F difference = GetPosition() - patrolOrigin;

	const float adjustedDistanceSquared = Craft::GetDistanceSquared(difference);
	const float patrolRadiusSquared = patrolRadius * patrolRadius;

	// 패트롤 범위 내에 들어오면 다시 패트롤 시작
	if (adjustedDistanceSquared <= patrolRadiusSquared)
	{
		ChangeEnemyState(EnemyState::Patrol);
		enemyState = EnemyState::Patrol;

		ResetPath();

		return;
	}

	
}
