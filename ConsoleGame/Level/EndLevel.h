#pragma once

#include <Level/Level.h>
#include <Resource/PixelImage.h>

class EndLevel : public Craft::Level
{
public:
	explicit EndLevel(int totalEarnedMoney);
	~EndLevel() override = default;

private:
	void OnInitialized() override;
	void Tick(float deltaTime) override;
	void Draw() override;

private:
	int totalEarnedMoney = 0;
	Craft::PixelImage resultScreenImage;
};
