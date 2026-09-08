#include <Actor/Enemy/NonHostile/MiniFish.h>

#include <Resource/TextImageLoader.h>
#include <Utility/Random.h>
#include <World/TileMap.h>


MiniFish::MiniFish(const Craft::Vector2F& position)
	: super(position)
{
	curState = EnemyState::Patrol;
	enemySpriteAnimation[static_cast<int>(EnemyState::Patrol)] = Craft::TextImageLoader::LoadAnimation(enemyPatrolFilename);
	enemySpriteAnimation[static_cast<int>(EnemyState::Return)] = Craft::TextImageLoader::LoadAnimation(enemyPatrolFilename);
	enemySpriteAnimation[static_cast<int>(EnemyState::Damaged)] = Craft::TextImageLoader::LoadAnimation(enemyDamagedFilename);
	enemySpriteAnimation[static_cast<int>(EnemyState::Flee)] = Craft::TextImageLoader::LoadAnimation(enemyFleeFilename);
	enemySpriteAnimation[static_cast<int>(EnemyState::Dead)] = Craft::TextImageLoader::LoadAnimation(enemyDeadFilename);

	ChangeImage(enemySpriteAnimation[static_cast<int>(curState)][0]);

	InitEnemy();
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
	if ((curState == EnemyState::Patrol || curState == EnemyState::Return) && DetectTarget())
	{
		ChangeEnemyState(EnemyState::Flee);

		ResetPath();
	}

	
	// 디버그 경로 출력
	if (auto map = tileMap.lock())
	{
		map->QueueDebugPath(movePath, currentPathIndex);
	}

}

void MiniFish::UpdateState(float deltaTime)
{
	super::UpdateState(deltaTime);

	switch (curState)
	{
	case EnemyState::Patrol:
		UpdatePatrol(deltaTime);
		break;
	case EnemyState::Flee:
		UpdateFlee(deltaTime);
		break;
	case EnemyState::Damaged:
		UpdateDamaged(deltaTime);
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
	if (movePath.empty())
	{
		if (enemyWaitTimer.IsTimeOut())
		{
			Craft::Vector2F PatrolPosition = GetPosition();
			if (FindRandomPatrolPoint(PatrolPosition, patrolRadius))
			{
				// 랜덤 위치 찾을때 이미 검증을 하므로 검증 안해도됨
				FindPathTo(PatrolPosition);
				enemyWaitTimer.SetTargetTime(Utility::RandomRange(0.f, patrolRetryInterval));
			}
			enemyWaitTimer.Reset();
		}
	}
	else
	{
		FollowPath(deltaTime);
	}
}

void MiniFish::UpdateFlee(float deltaTime)
{
	ResetPath();

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

void MiniFish::UpdateDamaged(float deltaTime)
{
	if (enemyWaitTimer.IsTimeOut())
	{
		// 피격 애니메이션이 끝나면 도망상태로 
		ChangeEnemyState(EnemyState::Flee);
	}
}

void MiniFish::UpdateReturn(float deltaTime)
{

	if (movePath.empty())
	{
		if (enemyWaitTimer.IsTimeOut())
		{
			if (FindReturnPath())
			{
				enemyWaitTimer.SetTargetTime(Utility::RandomRange(0.f, patrolRetryInterval));
			}
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

		ResetPath();

		return;
	}
}

void MiniFish::UpdateDead(float deltaTime)
{
	// 체크함수는 충돌체크 함수에다가 넣어주면 될거같다.
	
	ResetPath();

	if (enemyWaitTimer.IsTimeOut())
	{
		DropItem();
		Destroy();
	}

}

void MiniFish::InitEnemy()
{
	super::InitEnemy();

	// 이동속도
	enemyMoveSpeed = 30.f;

	// 체력
	Hp = 100.f;
	isDamaged = false;
	isDead = false;

	// 감지 범위
	detectRadius = 100.f;

	// 패트롤 범위
	patrolRadius = 60.f;

	// 다시 패트롤하는 쿨타임 랜덤설정할거임
	patrolRetryInterval = 0.5f;

	// 이 거리가 넘으면 도망 종료
	fleeEndDistance = 300.f;

	enemyWaitTimer.SetTargetTime(Utility::RandomRange(0.f, patrolRetryInterval));
}
