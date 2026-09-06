#pragma once

#include <Actor/Actor.h>

class EnemyAttackBox : public Craft::Actor
{
	TYPE_DECLARATIONS(EnemyAttackBox, Actor)

public:
	EnemyAttackBox(const Craft::Vector2F& position, const Craft::Vector2F& size, float damage);
	~EnemyAttackBox() = default;

private:

	virtual void Draw() override;
	virtual void Tick(float deltaTime) override;
	virtual void OnCollision(const std::shared_ptr<Craft::Actor>& other) override;

private:

	Craft::PixelImage debugImage;

	Craft::Vector2F size;

	float damage = 0.f;

	float elapsedTime = 1.f;
};

