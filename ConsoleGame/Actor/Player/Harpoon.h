#pragma once

#include <Actor/Actor.h>

#include <numbers>

class Harpoon : public Craft::Actor
{
	TYPE_DECLARATIONS(Harpoon, Actor)

public:
	Harpoon(const Craft::Vector2F& start, const Craft::Vector2F& mousePosition, float damage);
	~Harpoon() = default;

	virtual void Tick(float deltaTime) override;

	// getter / setter
	Craft::Vector2F GetSegmentStart() const { return segmentStart; }
	Craft::Vector2F GetSegmentEnd() const { return segmentEnd; }
	float GetDamage() const { return damage; }

public:
	// 사거리
	inline static constexpr float attackRadius = 100.f;

private:
	
	// 생명시간
	float remainingLifeTime = 0.3f;

	float damage = 0.f;

	Craft::Vector2F segmentStart = Craft::Vector2F::Zero;
	Craft::Vector2F segmentEnd = Craft::Vector2F::Zero;
};
