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

void MiniFish::UpdateState(float deltaTime)
{
	super::UpdateState(deltaTime);

	switch (enemyState)
	{
	case EnemyState::Patrol:
		UpdatePatrol(deltaTime);
		break;
	case EnemyState::Flee:
		UpdateFlee(deltaTime);
		break;
	case EnemyState::Return:
		UpdateReturn(deltaTime);
		break;
	case EnemyState::Dead:
		UpdateDead(deltaTime);
		break;
	}
}

void MiniFish::UpdatePatrol(float deltaTime)
{
	if (patrolPath.empty())
	{
		patrolWaitTime -= deltaTime;

		if (patrolWaitTime <= 0.f)
		{
			FindRandomPatrolPoint(patrolRadius);
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
	

	auto target = targetPtr.lock();
	auto map = tileMap.lock();

	if (!target || !map)
	{
		return;
	}

	const Craft::Vector2F difference = GetPosition() - target->GetPosition();

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

void MiniFish::UpdateDead(float deltaTime)
{
}
