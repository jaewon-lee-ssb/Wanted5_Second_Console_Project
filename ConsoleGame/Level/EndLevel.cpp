#include <Level/EndLevel.h>

#include <Level/GameLevel.h>
#include <UI/PixelText.h>

#include <Engine/Engine.h>
#include <Input/Input.h>
#include <Render/Renderer.h>

#include <algorithm>
#include <string>
#include <string_view>

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

	void DrawCenteredText(
		Craft::PixelImage& image,
		std::string_view text,
		int y,
		int scaleX,
		int scaleY,
		Craft::BackgroundColor color
	)
	{
		const int x = (image.width - GameUI::GetPixelTextWidth(text, scaleX)) / 2;
		GameUI::DrawPixelText(image, text, x, y, scaleX, scaleY, color);
	}
}

EndLevel::EndLevel(int totalEarnedMoney)
	: totalEarnedMoney((std::max)(0, totalEarnedMoney))
{
}

void EndLevel::OnInitialized()
{
	Level::OnInitialized();

	resultScreenImage = CreateSolidImage(
		Craft::Engine::Get().GetWidth(),
		Craft::Engine::Get().GetHeight(),
		Craft::BackgroundColor::Blue
	);

	DrawCenteredText(
		resultScreenImage,
		"DIVE COMPLETE",
		35,
		4,
		2,
		Craft::BackgroundColor::LightCyan
	);

	DrawCenteredText(
		resultScreenImage,
		"TOTAL EARNED",
		75,
		2,
		1,
		Craft::BackgroundColor::White
	);

	const std::string earnedMoneyText = std::to_string(totalEarnedMoney);
	DrawCenteredText(
		resultScreenImage,
		earnedMoneyText,
		95,
		4,
		2,
		Craft::BackgroundColor::Yellow
	);

	DrawCenteredText(
		resultScreenImage,
		"PRESS ANY KEY",
		145,
		2,
		1,
		Craft::BackgroundColor::LightGray
	);
}

void EndLevel::Tick(float deltaTime)
{
	Level::Tick(deltaTime);

	if (Craft::Input::Get().GetAnyKeyDown())
	{
		Craft::Engine::Get().AddNewLevel<GameLevel>();
	}
}

void EndLevel::Draw()
{
	Level::Draw();

	Craft::Renderer::Get().SubmitUI(
		resultScreenImage,
		Craft::Vector2F::Zero,
		Craft::Vector2F::Zero,
		9000
	);
}
