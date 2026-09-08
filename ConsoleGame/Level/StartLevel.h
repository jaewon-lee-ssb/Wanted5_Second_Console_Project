#pragma once

#include <Level/Level.h>
#include <Resource/PixelImage.h>

class StartLevel : public Craft::Level
{
public:
	StartLevel() = default;
	~StartLevel() override = default;

private:
	void OnInitialized() override;
	void Tick(float deltaTime) override;
	void Draw() override;

	bool IsStartButtonClicked() const;

private:
	Craft::PixelImage backgroundImage;
	Craft::PixelImage startButtonImage;
	Craft::Vector2F startButtonPosition = Craft::Vector2F::Zero;
};
