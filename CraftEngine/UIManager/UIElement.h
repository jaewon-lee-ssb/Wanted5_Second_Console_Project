#pragma once

#include <Core/Core.h>

#include <Math/Vector2.h>

namespace Craft
{
    class CRAFT_API UIElement
    {
    public:
        virtual ~UIElement() = default;

        virtual void Tick(float deltaTime)
        {
        }

        virtual void Draw() = 0;

        bool IsVisible() const
        {
            return isVisible;
        }

        void SetVisible(bool visible)
        {
            isVisible = visible;
        }

    protected:
        bool isVisible = true;
    };

}