#pragma once

#include <Actor/Actor.h>

class Player : public Craft::Actor
{
	TYPE_DECLARATIONS(Player, Actor)

public:
	Player(const Craft::Vector2F& position);
	~Player() = default;

	virtual void Tick(float deltaTime) override;
};

