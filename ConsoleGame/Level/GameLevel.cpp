#include <Level/GameLevel.h>

#include <Level/EndLevel.h>

#include <Actor/Player/Player.h>
#include <Actor/Enemy/EnemySpawner.h>

#include <Actor/Interaction/ExitZone.h>

#include <Inventory/Inventory.h>
#include <UI/InventoryUI.h>
#include <UI/GameHUD.h>

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
	this->player = player;
	player->SetTileMap(map);

	Craft::UIManager::Get().CreateUI<GameHUD>(player);

	if (GetCamera())
	{
		GetCamera()->SetTarget(player);
		
		GetCamera()->SetMapSize(Craft::Vector2F(static_cast<float>(map->GetWidth()), static_cast<float>(map->GetHeight())));
	}

	auto enemySpawner = SpawnActor<EnemySpawner>();
	enemySpawner->SetTileMap(map);

	auto SpawnExitZone = [&](const Craft::Vector2F& position)
		{
			auto exit = SpawnActor<ExitZone>(position);
			exit->SetPlayer(player);
			exitZones.emplace_back(exit);
		};

	SpawnExitZone({ 520.f, 42.f });
	SpawnExitZone({ 1356.f, 466.f });
	SpawnExitZone({ 492.f, 878.f });
	
	inventory = std::make_shared<Inventory>(4, 3);

	player->SetInventory(inventory);

	Craft::UIManager::Get().CreateUI<InventoryUI>(inventory, Craft::Vector2F(20.f, 10.f));

	gameTimer.SetTargetTime(gameDuration);
	gameTimer.Reset();

}

void GameLevel::BeginPlay()
{
	Level::BeginPlay();

	Craft::Engine::Get().PlayBackgroundMusic("deepsee.wav");
}

void GameLevel::Tick(float deltaTime)
{
	gameTimer.Tick(deltaTime);

	if (Craft::Input::Get().GetKeyDown('Q'))
	{
		isDebugMode = !isDebugMode;

		if (const std::shared_ptr<Craft::TileMap> map = tileMap.lock())
		{
			map->SetPathDebugEnabled(isDebugMode);
		}
	}

	Level::Tick(deltaTime);

	const std::shared_ptr<Player> lockedPlayer = player.lock();
	const bool isPlayerDead = !lockedPlayer || lockedPlayer->GetHealth() <= 0.f;
	
	if (gameTimer.IsTimeOut() || isPlayerDead)
	{
		RequestEndLevel();
	}

	for (const auto& weakExit : exitZones)
	{
		if (auto exit = weakExit.lock())
		{
			if (exit->HasEscapeRequested())
			{
				RequestEndLevel();
				return;
			}
		}
	}

}

void GameLevel::Draw()
{
	Level::Draw();
}

void GameLevel::RequestEndLevel()
{
	if (hasRequestedEndLevel)
	{
		return;
	}

	hasRequestedEndLevel = true;

	const int totalEarnedMoney = inventory
		? inventory->CalculateTotalSellPrice()
		: 0;

	Craft::Engine::Get().AddNewLevel<EndLevel>(totalEarnedMoney);
}
