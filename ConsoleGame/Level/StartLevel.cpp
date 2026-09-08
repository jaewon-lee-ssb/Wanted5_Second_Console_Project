#include <Level/StartLevel.h>

#include <Level/GameLevel.h>

#include <Engine/Engine.h>
#include <Input/Input.h>
#include <Render/Renderer.h>

#include <array>
#include <string_view>

namespace
{
	struct Glyph
	{
		char character = ' ';
		std::array<std::string_view, 5> rows;
	};

	constexpr std::array<Glyph, 9> glyphTable =
	{
		Glyph{ 'A', { "010", "101", "111", "101", "101" } },
		Glyph{ 'D', { "110", "101", "101", "101", "110" } },
		Glyph{ 'E', { "111", "100", "110", "100", "111" } },
		Glyph{ 'I', { "111", "010", "010", "010", "111" } },
		Glyph{ 'P', { "110", "101", "110", "100", "100" } },
		Glyph{ 'R', { "110", "101", "110", "101", "101" } },
		Glyph{ 'S', { "111", "100", "111", "001", "111" } },
		Glyph{ 'T', { "111", "010", "010", "010", "010" } },
		Glyph{ 'V', { "101", "101", "101", "101", "010" } }
	};

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

	const Glyph* FindGlyph(char character)
	{
		for (const Glyph& glyph : glyphTable)
		{
			if (glyph.character == character)
			{
				return &glyph;
			}
		}

		return nullptr;
	}

	int GetTextWidth(std::string_view text, int scaleX)
	{
		constexpr int glyphWidth = 3;
		constexpr int glyphSpacing = 1;

		if (text.empty())
		{
			return 0;
		}

		return static_cast<int>((glyphWidth * text.size() + glyphSpacing * (text.size() - 1)) * scaleX);
	}

	void DrawPixelText(
		Craft::PixelImage& image,
		std::string_view text,
		int startX,
		int startY,
		int scaleX,
		int scaleY,
		Craft::BackgroundColor color
	)
	{
		constexpr int glyphWidth = 3;
		constexpr int glyphSpacing = 1;

		for (int glyphIndex = 0; glyphIndex < static_cast<int>(text.size()); ++glyphIndex)
		{
			const Glyph* glyph = FindGlyph(text[glyphIndex]);
			if (glyph == nullptr)
			{
				continue;
			}

			for (int y = 0; y < 5; ++y)
			{
				for (int x = 0; x < glyphWidth; ++x)
				{
					if (glyph->rows[y][x] != '1')
					{
						continue;
					}

					for (int scaledY = 0; scaledY < scaleY; ++scaledY)
					{
						for (int scaledX = 0; scaledX < scaleX; ++scaledX)
						{
							const int pixelX = startX
								+ (glyphIndex * (glyphWidth + glyphSpacing) + x) * scaleX
								+ scaledX;
							const int pixelY = startY + y * scaleY + scaledY;

							Craft::Pixel& pixel = image.At(pixelX, pixelY);
							pixel.color = color;
							pixel.transparent = false;
						}
					}
				}
			}
		}
	}
}

void StartLevel::OnInitialized()
{
	Level::OnInitialized();

	const int screenWidth = Craft::Engine::Get().GetWidth();
	const int screenHeight = Craft::Engine::Get().GetHeight();

	backgroundImage = CreateSolidImage(screenWidth, screenHeight, Craft::BackgroundColor::Blue);
	startButtonImage = CreateSolidImage(60, 15, Craft::BackgroundColor::Cyan);

	constexpr std::string_view title = "DEEP DIVER";
	constexpr int titleScaleX = 4;
	constexpr int titleScaleY = 2;
	const int titleX = (screenWidth - GetTextWidth(title, titleScaleX)) / 2;
	const int titleY = screenHeight / 3 - 5;

	DrawPixelText(
		backgroundImage,
		title,
		titleX + 2,
		titleY + 1,
		titleScaleX,
		titleScaleY,
		Craft::BackgroundColor::DarkGray
	);

	DrawPixelText(
		backgroundImage,
		title,
		titleX,
		titleY,
		titleScaleX,
		titleScaleY,
		Craft::BackgroundColor::LightCyan
	);

	for (int y = 0; y < startButtonImage.height; ++y)
	{
		for (int x = 0; x < startButtonImage.width; ++x)
		{
			const bool isBorder = x < 2 || x >= startButtonImage.width - 2
				|| y == 0 || y == startButtonImage.height - 1;

			if (isBorder)
			{
				startButtonImage.At(x, y).color = Craft::BackgroundColor::LightCyan;
			}
		}
	}

	constexpr std::string_view startText = "START";
	constexpr int startTextScaleX = 2;
	const int startTextX = (startButtonImage.width - GetTextWidth(startText, startTextScaleX)) / 2;
	const int startTextY = (startButtonImage.height - 5) / 2;

	DrawPixelText(
		startButtonImage,
		startText,
		startTextX,
		startTextY,
		startTextScaleX,
		1,
		Craft::BackgroundColor::White
	);

	startButtonPosition.x = static_cast<float>((screenWidth - startButtonImage.width) / 2);
	startButtonPosition.y = static_cast<float>((screenHeight - startButtonImage.height) / 2);
}

void StartLevel::Tick(float deltaTime)
{
	Level::Tick(deltaTime);

	if (Craft::Input::Get().GetKeyDown(VK_RETURN) || IsStartButtonClicked())
	{
		Craft::Engine::Get().AddNewLevel<GameLevel>();
	}
}

void StartLevel::Draw()
{
	Level::Draw();

	Craft::Renderer::Get().SubmitUI(
		backgroundImage,
		Craft::Vector2F::Zero,
		Craft::Vector2F::Zero,
		9000
	);

	Craft::Renderer::Get().SubmitUI(startButtonImage, startButtonPosition);
}

bool StartLevel::IsStartButtonClicked() const
{
	if (!Craft::Input::Get().GetKeyDown(VK_LBUTTON))
	{
		return false;
	}

	const Craft::Vector2F& mousePosition = Craft::Input::Get().GetMousePosition();

	return mousePosition.x >= startButtonPosition.x
		&& mousePosition.x < startButtonPosition.x + startButtonImage.width
		&& mousePosition.y >= startButtonPosition.y
		&& mousePosition.y < startButtonPosition.y + startButtonImage.height;
}
