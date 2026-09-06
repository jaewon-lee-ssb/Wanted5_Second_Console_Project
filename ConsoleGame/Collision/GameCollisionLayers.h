#pragma once

#include <Physics/CollisionLayer.h>

namespace GameCollision
{
	constexpr Craft::CollisionLayer Player = Craft::Collision::Layer0;
	constexpr Craft::CollisionLayer PlayerAttack = Craft::Collision::Layer1;
	constexpr Craft::CollisionLayer Enemy = Craft::Collision::Layer2;
	constexpr Craft::CollisionLayer EnemyAttack = Craft::Collision::Layer3;
}