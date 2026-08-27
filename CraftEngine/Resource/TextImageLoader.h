#pragma once

#include <Core/Core.h>
#include <Resource/PixelImage.h>

#include <string>
#include <vector>

namespace Craft
{
    class CRAFT_API TextImageLoader
    {
    public:
        static std::vector<PixelImage> LoadAnimation(const std::string& filename);
    };
}