#pragma once

#include <Core/Core.h>
#include <Math/Color.h>

#include <vector>

namespace Craft
{
    struct CRAFT_API Pixel
    {
        BackgroundColor color = BackgroundColor::Black;
        bool transparent = true;
    };

    struct CRAFT_API PixelImage
    {
        int width = 0;
        int height = 0;
        std::vector<Pixel> pixels;

        Pixel& At(int x, int y)
        {
            return pixels.at(y * width + x);
        }

        const Pixel& At(int x, int y) const
        {
            return pixels.at(y * width + x);
        }
    };
}