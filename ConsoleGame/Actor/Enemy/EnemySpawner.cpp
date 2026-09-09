#include <Actor/Enemy/EnemySpawner.h>

#include <Actor/Enemy/Hostile/Pufferfish.h>
#include <Actor/Enemy/NonHostile/MiniFish.h>

#include <Item/ItemData.h>

#include <World/TileMap.h>
#include <Level/Level.h>
#include <Utility/Random.h>
#include <Math/Color.h>

namespace
{
	std::shared_ptr<const ItemData> CreateDropItemData(
		int id,
		const char* name,
		int inventoryWidth,
		int inventoryHeight,
		int sellPrice,
		ItemGrade grade,
		Craft::BackgroundColor color
	)
	{
		auto data = std::make_shared<ItemData>();

		data->id = id;
		data->name = name;
		data->inventoryWidth = inventoryWidth;
		data->inventoryHeight = inventoryHeight;
		data->sellPrice = sellPrice;
		data->grade = grade;

		data->image.width = inventoryWidth * 24;
		data->image.height = inventoryHeight * 10;
		data->image.pixels.resize(
			data->image.width * data->image.height
		);

		for (Craft::Pixel& pixel : data->image.pixels)
		{
			pixel.transparent = false;
			pixel.color = color;
		}

		return data;
	}
}

EnemySpawner::EnemySpawner()
	: super({})
{
	InitializeDropTables();
}

void EnemySpawner::InitializeDropTables()
{
	std::shared_ptr<const ItemData> commonFishData =
		CreateDropItemData(
			1,
			"Mini Fish Meat",
			1,
			1,
			100,
			ItemGrade::Common,
			Craft::BackgroundColor::LightCyan
		);

	std::shared_ptr<const ItemData> rareFishData =
		CreateDropItemData(
			2,
			"Rare Mini Fish Meat",
			2,
			1,
			500,
			ItemGrade::Rare,
			Craft::BackgroundColor::LightBlue
		);

	std::shared_ptr<const ItemData> epicFishData =
		CreateDropItemData(
			3,
			"Epic Mini Fish Meat",
			3,
			1,
			1000,
			ItemGrade::Epic,
			Craft::BackgroundColor::Purple
		);

	std::shared_ptr<const ItemData> legendaryFishData =
		CreateDropItemData(
			4,
			"Legendary Mini Fish Meat",
			2,
			2,
			2000,
			ItemGrade::Legendary,
			Craft::BackgroundColor::Yellow
		);

	miniFishDropTable =
	{
		{ commonFishData,    50 },
		{ rareFishData,      40 },
		{ epicFishData,      30 },
		{ legendaryFishData, 20 }
	};
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
			spawnBounds = MiniFish::GetSpawnBounds(spawnPoint);
		}
		else
		{
			spawnBounds = Pufferfish::GetSpawnBounds(spawnPoint);
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
			enemy->SetDropTable(miniFishDropTable);
		}
		else
		{
			auto enemy = GetOwner()->SpawnActor<Pufferfish>(spawnPoint);
			enemy->SetTileMap(map);
			enemy->SetDropTable(miniFishDropTable);
		}

		

		++curEnemyCount;
		return;
	}

}
