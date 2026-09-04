#pragma once

#include <Math/Vector2.h>

#include <memory>

namespace Craft
{
	class Actor;

	struct RaycastHit
	{
		std::shared_ptr<Actor> actor;
		Vector2F point = Vector2F::Zero;
		float t = 0.f;
	};
}