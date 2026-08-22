#pragma once

#include <Actor/Actor.h>

#include <numbers>

class Harpoon : public Craft::Actor
{
	TYPE_DECLARATIONS(Harpoon, Actor)

public:
	Harpoon(const Craft::Vector2F& start, const Craft::Vector2F& mousePosition);
	~Harpoon() = default;

	virtual void Tick(float deltaTime) override;


public:
	// 사거리
	inline static constexpr float attackRadius = 100.f;

private:
	

	// 생명시간
	float remainingLifeTime = 0.1f;
};
