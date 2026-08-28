#include <Actor/Enemy/NonHostile/MiniFish.h>

#include <Resource/TextImageLoader.h>

#include <World/TileMap.h>

MiniFish::MiniFish(const Craft::Vector2F& position)
	: super(position)
{
	enemySpriteAnimation[static_cast<int>(EnemyState::Patrol)] = Craft::TextImageLoader::LoadAnimation(enemyPatrolFilename);

	currentStateIndex = static_cast<int>(EnemyState::Patrol);
	ChangeImage(enemySpriteAnimation[currentStateIndex][0]);

	
}

void MiniFish::BeginPlay()
{
	super::BeginPlay();

	auto map = tileMap.lock();

	if (map)
	{
		enemyMoveSpeedX = 30.f;
		enemyMoveSpeedY = 15.f;
		patrolPath = map->FindPath(GetPosition(), Craft::Vector2F(600.f, 30.f), static_cast<float>(GetWidth()), static_cast<float>(GetHeight()));
	}
}

void MiniFish::Tick(float deltaTime)
{
	super::Tick(deltaTime);
	FollowPath(deltaTime);

	if (auto map = tileMap.lock())
	{
		map->QueueDebugPath(patrolPath, currentPathIndex);
	}

	animationElapsedTime += deltaTime;

	if (!enemySpriteAnimation[currentStateIndex].empty() && animationElapsedTime >= animationFrameTime)
	{
		animationElapsedTime -= animationFrameTime;

		currentAnimationSpriteIndex = (currentAnimationSpriteIndex + 1) % enemySpriteAnimation[currentStateIndex].size();

		ChangeImage(enemySpriteAnimation[currentStateIndex][currentAnimationSpriteIndex]);
	}
}

void MiniFish::FollowPath(float deltaTime)
{
	auto map = tileMap.lock();

	if (!map)
	{
		return;
	}

	if (currentPathIndex >= patrolPath.size())
	{
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
