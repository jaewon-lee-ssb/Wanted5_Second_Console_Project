#include <UI/PixelText.h>

#include <array>

namespace
{
	struct Glyph
	{
		char character;
		std::array<std::string_view, 5> rows;
	};

	constexpr std::array<Glyph, 36> glyphTable =
	{
		Glyph{ 'A', { "010", "101", "111", "101", "101" } },
		Glyph{ 'B', { "110", "101", "110", "101", "110" } },
		Glyph{ 'C', { "011", "100", "100", "100", "011" } },
		Glyph{ 'D', { "110", "101", "101", "101", "110" } },
		Glyph{ 'E', { "111", "100", "110", "100", "111" } },
		Glyph{ 'F', { "111", "100", "110", "100", "100" } },
		Glyph{ 'G', { "011", "100", "101", "101", "011" } },
		Glyph{ 'H', { "101", "101", "111", "101", "101" } },
		Glyph{ 'I', { "111", "010", "010", "010", "111" } },
		Glyph{ 'J', { "001", "001", "001", "101", "010" } },
		Glyph{ 'K', { "101", "101", "110", "101", "101" } },
		Glyph{ 'L', { "100", "100", "100", "100", "111" } },
		Glyph{ 'M', { "101", "111", "111", "101", "101" } },
		Glyph{ 'N', { "101", "111", "111", "111", "101" } },
		Glyph{ 'O', { "010", "101", "101", "101", "010" } },
		Glyph{ 'P', { "110", "101", "110", "100", "100" } },
		Glyph{ 'Q', { "010", "101", "101", "111", "011" } },
		Glyph{ 'R', { "110", "101", "110", "101", "101" } },
		Glyph{ 'S', { "011", "100", "010", "001", "110" } },
		Glyph{ 'T', { "111", "010", "010", "010", "010" } },
		Glyph{ 'U', { "101", "101", "101", "101", "111" } },
		Glyph{ 'V', { "101", "101", "101", "101", "010" } },
		Glyph{ 'W', { "101", "101", "111", "111", "101" } },
		Glyph{ 'X', { "101", "101", "010", "101", "101" } },
		Glyph{ 'Y', { "101", "101", "010", "010", "010" } },
		Glyph{ 'Z', { "111", "001", "010", "100", "111" } },
		Glyph{ '0', { "111", "101", "101", "101", "111" } },
		Glyph{ '1', { "010", "110", "010", "010", "111" } },
		Glyph{ '2', { "110", "001", "010", "100", "111" } },
		Glyph{ '3', { "110", "001", "010", "001", "110" } },
		Glyph{ '4', { "101", "101", "111", "001", "001" } },
		Glyph{ '5', { "111", "100", "110", "001", "110" } },
		Glyph{ '6', { "011", "100", "111", "101", "111" } },
		Glyph{ '7', { "111", "001", "010", "010", "010" } },
		Glyph{ '8', { "111", "101", "111", "101", "111" } },
		Glyph{ '9', { "111", "101", "111", "001", "110" } }
	};

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
}

int GameUI::GetPixelTextWidth(std::string_view text, int scaleX)
{
	constexpr int glyphWidth = 3;
	constexpr int glyphSpacing = 1;

	if (text.empty())
	{
		return 0;
	}

	return static_cast<int>(
		(glyphWidth * text.size() + glyphSpacing * (text.size() - 1)) * scaleX
	);
}

void GameUI::DrawPixelText(
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

						if (pixelX < 0 || pixelX >= image.width || pixelY < 0 || pixelY >= image.height)
						{
							continue;
						}

						Craft::Pixel& pixel = image.At(pixelX, pixelY);
						pixel.color = color;
						pixel.transparent = false;
					}
				}
			}
		}
	}
}
