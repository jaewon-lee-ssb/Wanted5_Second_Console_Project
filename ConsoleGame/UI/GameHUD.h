#pragma once

#include <UIManager/UIElement.h>
#include <Math/Vector2.h>
#include <Resource/PixelImage.h>

#include <memory>

class Player;

class GameHUD : public Craft::UIElement
{
public:
	explicit GameHUD(const std::shared_ptr<Player>& player);
	~GameHUD() override = default;

	void Tick(float deltaTime) override;
	void Draw() override;

private:
	void UpdateHealthBar(const Player& player);
	void UpdateCooldownBar(const Player& player);

private:
	std::weak_ptr<Player> player;

	Craft::Vector2F healthBarPosition = Craft::Vector2F::Zero;
	Craft::PixelImage healthBarFrameImage;
	Craft::PixelImage healthBarEmptyImage;
	Craft::PixelImage healthBarFillImage;

	Craft::Vector2F cooldownBarPosition = Craft::Vector2F::Zero;
	Craft::PixelImage cooldownBarFrameImage;
	Craft::PixelImage cooldownBarEmptyImage;
	Craft::PixelImage cooldownBarFillImage;

	int displayedHealthWidth = -1;
	int displayedCooldownWidth = -1;

	static constexpr int healthBarWidth = 80;
	static constexpr int healthBarHeight = 6;
	static constexpr int cooldownBarWidth = 80;
	static constexpr int cooldownBarHeight = 4;
	static constexpr int borderWidth = 2;
	static constexpr int borderHeight = 1;
	static constexpr int barSpacing = 2;
};
