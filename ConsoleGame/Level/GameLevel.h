#pragma once

#include <Level/Level.h>

class Inventory;

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
	std::weak_ptr<Craft::TileMap> tileMap;
	bool isDebugMode = false;
};

