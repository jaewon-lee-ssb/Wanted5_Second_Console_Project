#pragma once

#include <Math/Color.h>
#include <Resource/PixelImage.h>

#include <string_view>

namespace GameUI
{
	int GetPixelTextWidth(std::string_view text, int scaleX = 1);

	void DrawPixelText(
		Craft::PixelImage& image,
		std::string_view text,
		int startX,
		int startY,
		int scaleX,
		int scaleY,
		Craft::BackgroundColor color
	);
}
