#pragma once

#include <Actor/Actor.h>

#include <Item/DropEntry.h>

#include <vector>

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
	void InitializeDropTables();

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

	void SpawnEnemy();

private:
	// 스폰 쿨타임
	float spawnInterval = 0.2f;
	float spawnTimer = 0.f;

	float mapWidth = 0.f;
	float mapHeight = 0.f;

	// 적 수 관리
	int maxEnemyCount = 20;
	int curEnemyCount = 0;

	// 맵 약참조
	std::weak_ptr<Craft::TileMap> tileMap;

	std::vector<DropEntry> miniFishDropTable;
};

