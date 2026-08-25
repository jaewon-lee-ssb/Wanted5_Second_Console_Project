#include <Actor/Player/Harpoon.h>

#include <Collision/GameCollisionLayers.h>

#include <algorithm>
#include <cmath>

namespace
{
	Craft::Vector2F GetHarpoonTargetPosition(const Craft::Vector2F& start, const Craft::Vector2F& mousePosition, float attackRadius)
	{
		const int startX = static_cast<int>(std::floor(start.x));
		const int startY = static_cast<int>(std::floor(start.y));

		const Craft::Vector2F dir = mousePosition - start;

		const float cellAspectRatio = 2.f;

		const float correctedX = dir.x;
		const float correctedY = dir.y * cellAspectRatio;

		const float correctedLength = std::sqrt(correctedX + correctedY);

		const float horizontalSign = dir.x < 0.f ? -1.f : 1.f;

		const float localX = std::abs(correctedX);

		float mouseAngle = std::atan2(correctedY, localX);

		const float maxAngle = 30.f * std::numbers::pi_v<float> / 180.f;
		const float clampedAngle = (std::clamp)(mouseAngle, -maxAngle, maxAngle);

		Craft::Vector2F fireDirection(std::cos(clampedAngle) * horizontalSign, std::sin(clampedAngle) / cellAspectRatio);

		return Craft::Vector2F(start + fireDirection * attackRadius);
	}

	Craft::PixelImage CreateHarpoonImage(const Craft::Vector2F& start, const Craft::Vector2F& mousePosition, const float attackRadius)
	{
		const int startX = static_cast<int>(std::floor(start.x));
		const int startY = static_cast<int>(std::floor(start.y));

		const Craft::Vector2F dir = mousePosition - start;

		const Craft::Vector2F targetPosition = GetHarpoonTargetPosition(start, mousePosition, attackRadius);
		
		const int endX = static_cast<int>(std::floor(targetPosition.x));
		const int endY = static_cast<int>(std::floor(targetPosition.y));

		const int minX = (std::min)(startX, endX);
		const int minY = (std::min)(startY, endY);

		Craft::PixelImage image;
		image.width = std::abs(endX - startX) + 1;
		image.height = std::abs(endY - startY) + 1;
		image.pixels.resize(image.width * image.height);

		int x = startX;
		int y = startY;
		const int deltaX = std::abs(endX - startX);
		const int deltaY = -std::abs(endY - startY);
		const int stepX = startX < endX ? 1 : -1;
		const int stepY = startY < endY ? 1 : -1;
		int error = deltaX + deltaY;

		while (true)
		{
			Craft::Pixel& pixel = image.At(x - minX, y - minY);
			pixel.color = Craft::BackgroundColor::LightCyan;
			pixel.transparent = false;

			if (x == endX && y == endY)
			{
				break;
			}

			const int doubleError = error * 2;
			if (doubleError >= deltaY)
			{
				error += deltaY;
				x += stepX;
			}
			if (doubleError <= deltaX)
			{
				error += deltaX;
				y += stepY;
			}
		}

		return image;
	}

	Craft::Vector2F GetHarpoonCenter(const Craft::Vector2F& start, const Craft::Vector2F& mousePosition, const float attackRadius)
	{
		const int startX = static_cast<int>(std::floor(start.x));
		const int startY = static_cast<int>(std::floor(start.y));

		const Craft::Vector2F targetPosition = GetHarpoonTargetPosition(start, mousePosition, attackRadius);

		Craft::Vector2F harpoonDirection = targetPosition - start;

		harpoonDirection.x = std::floor(harpoonDirection.x);
		harpoonDirection.y = std::floor(harpoonDirection.y);

		return Craft::Vector2F(startX + harpoonDirection.x * 0.5f, startY + harpoonDirection.y * 0.5f);
	}
}

Harpoon::Harpoon(const Craft::Vector2F& start, const Craft::Vector2F& mousePosition)
	: Actor(CreateHarpoonImage(start, mousePosition, attackRadius), GetHarpoonCenter(start, mousePosition, attackRadius), Utility::ActorTags::PlayerAttackBox)
{
	SetCollisionLayer(GameCollision::PlayerAttack);

}

void Harpoon::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	remainingLifeTime -= deltaTime;
	if (remainingLifeTime <= 0.0f)
	{
		Destroy();
	}
}
