#pragma once

#include <Actor/Actor.h>

class ExitZone : public Craft::Actor
{
	TYPE_DECLARATIONS(ExitZone, Actor)

public:
	ExitZone(const Craft::Vector2F& position);
	~ExitZone() = default;

	bool HasEscapeRequested() const { return escapeRequested; }

	void SetPlayer(const std::shared_ptr<Actor>& player) { playerPtr = player; }

private:

	virtual void Tick(float deltaTime) override;

	bool IsPlayerInside();

private:
	std::weak_ptr<Actor> playerPtr;

	float escapeHoldTime = 0.f;
	float requiredHoldTime = 1.f;

	bool escapeRequested = false;

	float interactionRadius = 6.f;

	const std::string exitzoneFilename = "Actors/Interaction/exitzone.txt";

};

