#include <Actor/Enemy/EnemySpawner.h>

#include <Actor/Enemy/NonHostile/MiniFish.h>

#include <World/TileMap.h>
#include <Utility/Random.h>

EnemySpawner::EnemySpawner()
	: super({}, Craft::Vector2F(300.f, 200.f))
{

}

void EnemySpawner::Tick(float deltaTime)
{
	super::Tick(deltaTime);


	if (curEnemyCount < maxEnemyCount)
	{
		SpawnEnemy();
	}
}

void EnemySpawner::SpawnEnemy()
{
	const Craft::Vector2F spawnPoint = Craft::Vector2F(GetPosition().x + Utility::RandomRange(-spawnRadius * 2, spawnRadius * 2), 
														GetPosition().y + Utility::RandomRange(-spawnRadius, spawnRadius));
	
	auto map = tileMap.lock();
	if (!map)
	{
		return;
	}

	map->OverlapsSolid()

}
