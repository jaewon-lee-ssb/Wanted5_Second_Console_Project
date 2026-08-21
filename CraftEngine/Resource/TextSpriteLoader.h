#pragma once

#include <Core/Core.h>
#include <Resource/PixelSprite.h>

#include <string>

namespace Craft
{
    class CRAFT_API TextSpriteLoader
    {
    public:
        static PixelSprite Load(const std::string& path);
    };
}