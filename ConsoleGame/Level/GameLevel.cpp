#include <Level/GameLevel.h>

#include <Actor/Player/Player.h>
#include <Actor/Enemy/EnemySpawner.h>

#include <Inventory/Inventory.h>
#include <UI/InventoryUI.h>

#include <Item/Item.h>
#include <Item/ItemData.h>
#include <Item/ItemPickup.h>

#include <Engine/Engine.h>
#include <Input/Input.h>
#include <Camera/Camera.h>
#include <World/TileMap.h>
#include <UIManager/UIManager.h>

void GameLevel::OnInitialized()
{
	Level::OnInitialized();

	auto map = SpawnActor<Craft::TileMap>();
	tileMap = map;

	if (!map->Load("Maps/Map1.txt"))
	{
		Craft::Engine::Get().Quit();
	}

	map->SetPathDebugEnabled(isDebugMode);

	auto player = SpawnActor<Player>(Craft::Vector2F(100.f, 30.f));
	player->SetTileMap(map);

	if (GetCamera())
	{
		GetCamera()->SetTarget(player);
		
		GetCamera()->SetMapSize(Craft::Vector2F(static_cast<float>(map->GetWidth()), static_cast<float>(map->GetHeight())));
	}

	auto enemySpawner = SpawnActor<EnemySpawner>();
	enemySpawner->SetTileMap(map);

	//auto enemy = SpawnActor<Pufferfish>(Craft::Vector2F(100.f, 50.f));
	//enemy->SetTileMap(map);
	
	inventory = std::make_shared<Inventory>(8, 6);

	player->SetInventory(inventory);

	

	Craft::UIManager::Get().CreateUI<InventoryUI>(inventory, Craft::Vector2F(20.f, 10.f));



}

void GameLevel::BeginPlay()
{
	Level::BeginPlay();

	
}

void GameLevel::Tick(float deltaTime)
{
	if (Craft::Input::Get().GetKeyDown('Q'))
	{
		isDebugMode = !isDebugMode;

		if (const std::shared_ptr<Craft::TileMap> map = tileMap.lock())
		{
			map->SetPathDebugEnabled(isDebugMode);
		}
	}

	Level::Tick(deltaTime);

	
}

void GameLevel::Draw()
{
	Level::Draw();
}
