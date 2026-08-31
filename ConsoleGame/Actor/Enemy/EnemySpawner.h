#pragma once

#include <Actor/Actor.h>

namespace Craft
{
	class TileMap;
}

class EnemySpawner : public Craft::Actor
{
	TYPE_DECLARATIONS(EnemySpawner, Actor)
		
public:
	EnemySpawner();
	~EnemySpawner() = default;

	void SetTileMap(std::shared_ptr<Craft::TileMap>& map) { tileMap = map; }

private:
	virtual void Tick(float deltaTime) override;

	void SpawnEnemy();

private:
	float spawnInterval = 5.f;
	float spawnTimer = 0.f;

	float spawnRadius = 100.f;

	int maxEnemyCount = 10;
	int curEnemyCount = 0;

	std::weak_ptr<Craft::TileMap> tileMap;
};

