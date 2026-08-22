#include <Level/GameLevel.h>

#include <Actor/Player/Player.h>

#include <Camera/Camera.h>

void GameLevel::OnInitialized()
{
	Level::OnInitialized();

	auto player = SpawnActor<Player>(Craft::Vector2F(350.0f, 125.0f));

	if (GetCamera())
	{
		GetCamera()->SetTarget(player);
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
