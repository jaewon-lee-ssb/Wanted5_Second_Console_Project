#include <UI/GameHUD.h>

#include <Actor/Player/Player.h>

#include <Engine/Engine.h>
#include <Render/Renderer.h>

#include <algorithm>

namespace
{
	Craft::PixelImage CreateSolidImage(int width, int height, Craft::BackgroundColor color)
	{
		Craft::PixelImage image;
		image.width = width;
		image.height = height;
		image.pixels.resize(width * height);

		for (Craft::Pixel& pixel : image.pixels)
		{
			pixel.color = color;
			pixel.transparent = false;
		}

		return image;
	}
}

GameHUD::GameHUD(const std::shared_ptr<Player>& player)
	: player(player)
{
	healthBarFrameImage = CreateSolidImage(
		healthBarWidth + borderWidth * 2,
		healthBarHeight + borderHeight * 2,
		Craft::BackgroundColor::LightGray
	);

	healthBarEmptyImage = CreateSolidImage(
		healthBarWidth,
		healthBarHeight,
		Craft::BackgroundColor::Black
	);

	cooldownBarFrameImage = CreateSolidImage(
		cooldownBarWidth + borderWidth * 2,
		cooldownBarHeight + borderHeight * 2,
		Craft::BackgroundColor::LightGray
	);

	cooldownBarEmptyImage = CreateSolidImage(
		cooldownBarWidth,
		cooldownBarHeight,
		Craft::BackgroundColor::Black
	);

	healthBarPosition.x = 10.f;
	healthBarPosition.y = static_cast<float>(
		Craft::Engine::Get().GetHeight()
		- healthBarFrameImage.height
		- cooldownBarFrameImage.height
		- barSpacing
		- 6
	);

	cooldownBarPosition.x = healthBarPosition.x;
	cooldownBarPosition.y = healthBarPosition.y
		+ healthBarFrameImage.height
		+ barSpacing;

	if (player)
	{
		UpdateHealthBar(*player);
		UpdateCooldownBar(*player);
	}
}

void GameHUD::Tick(float deltaTime)
{
	std::shared_ptr<Player> lockedPlayer = player.lock();
	if (!lockedPlayer)
	{
		return;
	}

	UpdateHealthBar(*lockedPlayer);
	UpdateCooldownBar(*lockedPlayer);
}

void GameHUD::Draw()
{
	if (player.expired())
	{
		return;
	}

	const Craft::Vector2F innerPosition(
		healthBarPosition.x + borderWidth,
		healthBarPosition.y + borderHeight
	);

	Craft::Renderer::Get().SubmitUI(
		healthBarFrameImage,
		healthBarPosition,
		Craft::Vector2F::Zero,
		10000
	);

	Craft::Renderer::Get().SubmitUI(
		healthBarEmptyImage,
		innerPosition,
		Craft::Vector2F::Zero,
		10001
	);

	if (!healthBarFillImage.pixels.empty())
	{
		Craft::Renderer::Get().SubmitUI(
			healthBarFillImage,
			innerPosition,
			Craft::Vector2F::Zero,
			10002
		);
	}

	const Craft::Vector2F cooldownInnerPosition(
		cooldownBarPosition.x + borderWidth,
		cooldownBarPosition.y + borderHeight
	);

	Craft::Renderer::Get().SubmitUI(
		cooldownBarFrameImage,
		cooldownBarPosition,
		Craft::Vector2F::Zero,
		10000
	);

	Craft::Renderer::Get().SubmitUI(
		cooldownBarEmptyImage,
		cooldownInnerPosition,
		Craft::Vector2F::Zero,
		10001
	);

	if (!cooldownBarFillImage.pixels.empty())
	{
		Craft::Renderer::Get().SubmitUI(
			cooldownBarFillImage,
			cooldownInnerPosition,
			Craft::Vector2F::Zero,
			10002
		);
	}
}

void GameHUD::UpdateHealthBar(const Player& player)
{
	const float maxHealth = player.GetMaxHealth();
	const float healthRatio = maxHealth > 0.f
		? std::clamp(player.GetHealth() / maxHealth, 0.f, 1.f)
		: 0.f;

	const int healthWidth = static_cast<int>(healthBarWidth * healthRatio);
	if (healthWidth == displayedHealthWidth)
	{
		return;
	}

	displayedHealthWidth = healthWidth;
	healthBarFillImage = CreateSolidImage(
		healthWidth,
		healthBarHeight,
		Craft::BackgroundColor::LightRed
	);
}

void GameHUD::UpdateCooldownBar(const Player& player)
{
	const float cooldownProgress = std::clamp(
		player.GetAttackCooldownProgress(),
		0.f,
		1.f
	);

	const int cooldownWidth = static_cast<int>(cooldownBarWidth * cooldownProgress);
	if (cooldownWidth == displayedCooldownWidth)
	{
		return;
	}

	displayedCooldownWidth = cooldownWidth;
	cooldownBarFillImage = CreateSolidImage(
		cooldownWidth,
		cooldownBarHeight,
		Craft::BackgroundColor::LightCyan
	);
}
