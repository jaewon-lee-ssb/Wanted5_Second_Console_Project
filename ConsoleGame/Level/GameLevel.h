#pragma once

#include <Level/Level.h>
#include <Utility/Timer.h>

class Inventory;
class Player;

namespace Craft
{
	class TileMap;
}

class GameLevel : public Craft::Level
{
public:
	GameLevel() = default;
	~GameLevel() = default;

private:
	virtual void OnInitialized() override;
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

private:
	std::shared_ptr<Inventory> inventory;
	std::weak_ptr<Player> player;
	std::weak_ptr<Craft::TileMap> tileMap;
	Utility::Timer gameTimer;
	bool isDebugMode = false;
	bool hasRequestedEndLevel = false;

	static constexpr float gameDuration = 180.f;
};

