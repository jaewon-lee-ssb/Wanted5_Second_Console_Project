#pragma once

#include <Core/Core.h>

#include <Windows.h>

namespace Craft
{
	// 글자 색상을 나타내는 열거형
	enum class CRAFT_API Color : WORD
	{
        Black = 0,

        Blue = FOREGROUND_BLUE,
        Green = FOREGROUND_GREEN,
        Red = FOREGROUND_RED,

        Brown = Red | Green,
        Purple = Red | Blue,
        Cyan = Green | Blue,

        LightGray = Red | Green | Blue,

        DarkGray = FOREGROUND_INTENSITY,

        LightBlue = Blue | FOREGROUND_INTENSITY,
        LightGreen = Green | FOREGROUND_INTENSITY,
        LightRed = Red | FOREGROUND_INTENSITY,

        Yellow = Red | Green | FOREGROUND_INTENSITY,
        LightCyan = Green | Blue | FOREGROUND_INTENSITY,
        LightMagenta = Red | Blue | FOREGROUND_INTENSITY,

        White = Red | Green | Blue | FOREGROUND_INTENSITY,
	};

    enum class CRAFT_API BackgroundColor : WORD
    {
        Black = 0,

        Blue = BACKGROUND_BLUE,
        Green = BACKGROUND_GREEN,
        Red = BACKGROUND_RED,

        Brown = Red | Green,
        Purple = Red | Blue,
        Cyan = Green | Blue,

        LightGray = Red | Green | Blue,

        DarkGray = BACKGROUND_INTENSITY,

        LightBlue = Blue | BACKGROUND_INTENSITY,
        LightGreen = Green | BACKGROUND_INTENSITY,
        LightRed = Red | BACKGROUND_INTENSITY,

        Yellow = Red | Green | BACKGROUND_INTENSITY,
        LightCyan = Green | Blue | BACKGROUND_INTENSITY,
        LightMagenta = Red | Blue | BACKGROUND_INTENSITY,

        White = Red | Green | Blue | BACKGROUND_INTENSITY,
    };

    inline BackgroundColor ParseColor(char ch)
    {
        int value = 0;

        if (ch >= '0' && ch <= '9')
            value = ch - '0';

        if (ch >= 'A' && ch <= 'F')
            value =  ch - 'A' + 10;

        return static_cast<BackgroundColor>(value << 4);
    }
}