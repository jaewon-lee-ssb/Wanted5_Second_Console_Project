#include <Actor/Enemy/Hostile/EnemyAttackBox.h>

#include <Actor/Player/Player.h>

#include <Collision/GameCollisionLayers.h>
#include <Level/Level.h>
#include <Camera/Camera.h>
#include <Render/Renderer.h>

namespace
{
	Craft::PixelImage CreateAttackBoxImage(const Craft::Vector2F& size)
	{
		Craft::PixelImage image;
		image.width = static_cast<int>(size.x);
		image.height = static_cast<int>(size.y);

		if (image.width <= 0 || image.height <= 0)
		{
			return image;
		}

		image.pixels.resize(
			static_cast<size_t>(image.width) * static_cast<size_t>(image.height));

		for (int y = 0; y < image.height; ++y)
		{
			for (int x = 0; x < image.width; ++x)
			{
				const bool isBorder =
					x == 0 || x == image.width - 1 ||
					y == 0 || y == image.height - 1;

				if (!isBorder)
				{
					continue;
				}

				Craft::Pixel& pixel = image.At(x, y);
				pixel.color = Craft::BackgroundColor::Yellow;
				pixel.transparent = false;
			}
		}

		return image;
	}
}

EnemyAttackBox::EnemyAttackBox(const Craft::Vector2F& position, const Craft::Vector2F& size, float damage)
	: super({}, position), debugImage(CreateAttackBoxImage(size)), size(size), damage(damage)
{
	SetCollisionLayer(GameCollision::EnemyAttack);
	SetCollisionMask(GameCollision::Player);

	pivot.x = size.x / 2.f;
	pivot.y = size.y / 2.f;
}

void EnemyAttackBox::Draw()
{
	/*if (!IsActive())
	{
		return;
	}

	const auto owner = GetOwner();
	if (!owner || !owner->GetCamera())
	{
		return;
	}

	const auto& camera = owner->GetCamera();
	const Craft::Vector2F debugPivot(
		debugImage.width / 2.f,
		debugImage.height / 2.f);

	Craft::Renderer::Get().SubmitWorld(
		debugImage,
		camera->WorldToScreen(GetPosition()),
		false,
		debugPivot,
		sortingOrder,
		camera->GetZoom());*/
}

void EnemyAttackBox::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	elapsedTime -= deltaTime;

	if (elapsedTime <= 0.f)
	{
		Destroy();
	}
}

void EnemyAttackBox::OnCollision(const std::shared_ptr<Craft::Actor>& other)
{
	super::OnCollision(other);

	if (!other)
	{
		return;
	}

	if ((other->GetCollisionLayer() & GameCollision::Player) == 0)
	{
		return;
	}

	Cast<Player>(other)->TakeDamage(damage);

}
