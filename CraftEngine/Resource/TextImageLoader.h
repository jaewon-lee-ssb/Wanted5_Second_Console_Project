#pragma once

#include <Core/Core.h>
#include <Resource/PixelImage.h>

#include <string>

namespace Craft
{
    class CRAFT_API TextImageLoader
    {
    public:
        static PixelImage Load(const std::string& filename);
    };
}