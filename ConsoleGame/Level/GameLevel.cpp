#include <Level/GameLevel.h>

#include <Actor/Player/Player.h>

void GameLevel::OnInitialized()
{
	Level::OnInitialized();

	SpawnActor<Player>(Craft::Vector2F(350.0f, 125.0f));
}

void GameLevel::Tick(float deltaTime)
{
	Level::Tick(deltaTime);
}

void GameLevel::Draw()
{
	Level::Draw();
}
