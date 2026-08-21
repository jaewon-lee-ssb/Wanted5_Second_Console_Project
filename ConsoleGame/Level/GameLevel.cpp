#include <Level/GameLevel.h>

void GameLevel::OnInitialized()
{
	Level::OnInitialized();


	std::vector<std::string> playerImage(
		20,
		std::string(30, '#')
	);

	SpawnActor<Craft::Actor>(
		playerImage,
		Craft::Vector2F(350.0f, 125.0f),
		Utility::ActorTags::Player,
		Craft::Color::White,
		Craft::BackgroundColor::Blue,
		Craft::Vector2F(15.f, 10.0f)
	);
}

void GameLevel::Tick(float deltaTime)
{
	Level::Tick(deltaTime);
}

void GameLevel::Draw()
{
	Level::Draw();
}
