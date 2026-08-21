#pragma once

#include <Level/Level.h>


class GameLevel : public Craft::Level
{
public:
	GameLevel() = default;
	~GameLevel() = default;

private:
	virtual void OnInitialized() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

private:
};

