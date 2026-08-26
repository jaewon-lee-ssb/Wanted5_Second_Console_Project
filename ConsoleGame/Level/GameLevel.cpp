#include <Level/GameLevel.h>

#include <Actor/Player/Player.h>

#include <Engine/Engine.h>
#include <Camera/Camera.h>
#include <World/TileMap.h>

void GameLevel::OnInitialized()
{
	Level::OnInitialized();

	auto map = SpawnActor<Craft::TileMap>();

	if (!map->Load("Maps/Map1.txt"))
	{
		Craft::Engine::Get().Quit();
	}

	auto player = SpawnActor<Player>(Craft::Vector2F(100.f, 10.f));
	player->SetTileMap(map);

	if (GetCamera())
	{
		GetCamera()->SetTarget(player);
		
		GetCamera()->SetMapSize(Craft::Vector2F(map->GetWidth(), map->GetHeight()));
	}

	
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
