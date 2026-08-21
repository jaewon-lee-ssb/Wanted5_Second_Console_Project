#pragma once

#include <Actor/Actor.h>

//#include <Resource/PixelImage.h>

class Player : public Craft::Actor
{
	TYPE_DECLARATIONS(Player, Actor)

public:
	Player(const Craft::Vector2F& position);
	~Player() = default;

	virtual void Tick(float deltaTime) override;

private:

	Craft::PixelImage playerIdleImage;

	std::string playerIdleFilename = "Actors/Player/player_idle.txt";
};

