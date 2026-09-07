#pragma once

#include <cstdint>

namespace Craft
{
	using CollisionLayer = uint32_t;

	namespace Collision
	{
		constexpr CollisionLayer None	= 0;
		constexpr CollisionLayer Layer0 = 1u << 0;
		constexpr CollisionLayer Layer1 = 1u << 1;
		constexpr CollisionLayer Layer2 = 1u << 2;
		constexpr CollisionLayer Layer3 = 1u << 3;
		constexpr CollisionLayer Layer4 = 1u << 4;
	}
}