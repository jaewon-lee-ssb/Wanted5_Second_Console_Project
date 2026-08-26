#pragma once

#include <Core/Core.h>
#include <Math/Vector2.h>

#include <memory>

namespace Craft
{
	class Actor;

	class CRAFT_API Camera
	{
	public:
		Camera();
		virtual ~Camera() = default;

		void SetTarget(const std::shared_ptr<Actor>& target);

		Vector2F WorldToScreen(const Vector2F& world) const;
		Vector2F ScreenToWorld(const Vector2F& screen) const;

		inline const Vector2F GetPosition() const { return position; }
		inline void SetPosition(const Vector2F& newPosition) { position = newPosition; }
		inline void SetMapSize(const Vector2F& newMapSize) { mapSize = newMapSize; }

		virtual void Tick(float deltaTime);

	private:
		Vector2F position;
		
		std::weak_ptr<Actor> target;

		Vector2F offset;

		Vector2F mapSize = Vector2F::Zero;
		
	};

}

