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
	enemyMoveSpeedX = 30.f;
	enemyMoveSpeedY = 15.f;

	patrolOrigin = position;
	 
	// 패트롤 지연타임
	patrolRetryInterval = Utility::RandomRange(0.f, 0.5f);
}

void MiniFish::BeginPlay()
{
	super::BeginPlay();


}

void MiniFish::Tick(float deltaTime)
{
	super::Tick(deltaTime);
	
	
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
	}

	
	// 디버그 경로 출력
	if (auto map = tileMap.lock())
	{
		map->QueueDebugPath(patrolPath, currentPathIndex);
	}

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

void MiniFish::DetectPlayer()
{
	if (auto player = target.lock())
	{
		const Craft::Vector2F diff = player->GetPosition() - GetPosition();

		constexpr float verticalScale = 2.f;

		const float adjustedX = diff.x;
		const float adjustedY = diff.y * verticalScale;

		const float adjustedDistanceSquared = adjustedX * adjustedX + adjustedY * adjustedY;

		const float detectDistanceSquared = detectRadius * detectRadius;

		if (adjustedDistanceSquared <= detectDistanceSquared)
		{
			enemyState = EnemyState::Flee;
		}
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
		patrolPath.clear();
		currentPathIndex = 0;
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

	const Craft::Vector2F movement(direction.x * enemyMoveSpeedX * deltaTime, direction.y * enemyMoveSpeedY * deltaTime);

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
		patrolPath.clear();
		currentPathIndex = 0;
	}
}

void MiniFish::FindRandomPatrolPoint()
{
	const float randomTargetX = Utility::RandomRange(-patrolRadiusX, patrolRadiusX);
	const float randomTargetY = Utility::RandomRange(-patrolRadiusY, patrolRadiusY);

	if (auto map = tileMap.lock())
	{
		patrolPath = map->FindPath(GetPosition(), Craft::Vector2F(patrolOrigin.x + randomTargetX, patrolOrigin.y + randomTargetY), static_cast<float>(GetWidth()), static_cast<float>(GetHeight()));
	}
}

void MiniFish::UpdatePatrol(float deltaTime)
{



	if (patrolPath.empty())
	{
		patrolWaitTime -= deltaTime;

		if (patrolWaitTime <= 0.f)
		{
			FindRandomPatrolPoint();
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

}

void MiniFish::UpdateReturn(float deltaTime)
{

}
