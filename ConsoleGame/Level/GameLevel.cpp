#include <Level/GameLevel.h>

#include <Actor/Player/Player.h>
#include <Actor/Enemy/NonHostile/MiniFish.h>
#include <Actor/Enemy/Hostile/Pufferfish.h>
#include <Actor/Enemy/EnemySpawner.h>

#include <Inventory/Inventory.h>
#include <UI/InventoryUI.h>

#include <Item/Item.h>
#include <Item/ItemData.h>

#include <Engine/Engine.h>
#include <Camera/Camera.h>
#include <World/TileMap.h>
#include <UIManager/UIManager.h>

void GameLevel::OnInitialized()
{
	Level::OnInitialized();

	auto map = SpawnActor<Craft::TileMap>();

	if (!map->Load("Maps/Map1.txt"))
	{
		Craft::Engine::Get().Quit();
	}

	map->SetPathDebugEnabled(true);

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

	auto testFishData = std::make_shared<ItemData>();

	testFishData->id = 1;
	testFishData->name = "Mini Fish";
	testFishData->inventoryWidth = 2;
	testFishData->inventoryHeight = 1;
	testFishData->sellPrice = 100;

	testFishData->image.width = testFishData->inventoryWidth * 24;
	testFishData->image.height = testFishData->inventoryHeight * 10;
	testFishData->image.pixels.resize(testFishData->image.width * testFishData->image.height);

	for (Craft::Pixel& pixel : testFishData->image.pixels)
	{
		pixel.transparent = false;
		pixel.color = Craft::BackgroundColor::LightCyan;
	}


	auto testFish = std::make_shared<Item>(testFishData);
	auto testFish2 = std::make_shared<Item>(testFishData);

	inventory->AddItem(testFish);
	inventory->AddItem(testFish2);
	

	Craft::UIManager::Get().CreateUI<InventoryUI>(inventory, Craft::Vector2F(20.f, 10.f));



}

void GameLevel::BeginPlay()
{
	Level::BeginPlay();

	
}

void GameLevel::Tick(float deltaTime)
{
	Level::Tick(deltaTime);

	
}

void GameLevel::Draw()
{
	Level::Draw();
}
