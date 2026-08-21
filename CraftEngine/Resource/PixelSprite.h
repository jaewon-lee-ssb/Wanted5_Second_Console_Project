#pragma once

#include <Core/Core.h>
#include <Math/Color.h>

#include <vector>

namespace Craft
{
    struct CRAFT_API PixelSprite
    {
        int Width;
        int Height;
        std::vector<Craft::Color> Pixels;
    };
}