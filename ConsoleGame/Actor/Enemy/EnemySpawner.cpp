#include <Actor/Enemy/EnemySpawner.h>

#include <Actor/Enemy/Hostile/Pufferfish.h>
#include <Actor/Enemy/NonHostile/MiniFish.h>

#include <World/TileMap.h>
#include <Level/Level.h>
#include <Utility/Random.h>

EnemySpawner::EnemySpawner()
	: super({})
{
	
}

void EnemySpawner::BeginPlay()
{
	if (auto map = tileMap.lock())
	{
		Craft::Vector2F mapSize = map->GetMapSize();
		mapWidth = mapSize.x;
		mapHeight = mapSize.y;
	}
}

void EnemySpawner::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	spawnTimer -= deltaTime;

	if (spawnTimer <= 0.f)
	{
		if (curEnemyCount < maxEnemyCount)
		{
			SpawnEnemy();
			spawnTimer = spawnInterval;
		}
	}
}

void EnemySpawner::SpawnEnemy()
{
	auto map = tileMap.lock();
	if (!map)
	{
		return;
	}

	constexpr int maxAttempts = 10;
	
	const float enemySpawnIndex = Utility::RandomRange(0.f, 10.f);
	//const float enemySpawnIndex = 8.f;

	for (int attempt = 0; attempt < maxAttempts; ++attempt)
	{
		const Craft::Vector2F spawnPoint(Utility::RandomRange(0.f, mapWidth), Utility::RandomRange(0.f, mapHeight));

		Craft::Bounds spawnBounds; 
		
		if (enemySpawnIndex < 7.f)
		{
			spawnBounds = Pufferfish::GetSpawnBounds(spawnPoint);
		}
		else
		{
			spawnBounds = MiniFish::GetSpawnBounds(spawnPoint);
		}

		if (!map->CanOccupyWorld(spawnBounds))
		{
			// 생성위치에 몬스터가 들어갈수 없다면
			continue;
		}

		if (enemySpawnIndex < 7.f)
		{
			auto enemy = GetOwner()->SpawnActor<MiniFish>(spawnPoint);
			enemy->SetTileMap(map);
		}
		else
		{
			auto enemy = GetOwner()->SpawnActor<Pufferfish>(spawnPoint);
			enemy->SetTileMap(map);
		}

		

		++curEnemyCount;
		return;
	}

}
