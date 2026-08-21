#pragma once

#include <Core/Core.h>
#include <Math/Color.h>

#include <vector>

namespace Craft
{
    struct CRAFT_API PixelImage
    {
        int width = 0;
        int height = 0;
        std::vector<Craft::Color> Pixels;
    };
}