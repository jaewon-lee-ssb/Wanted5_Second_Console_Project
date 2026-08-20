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

    // 나중에 픽셀 렌더러를 만들게 된다면 사용할수 있다
    //enum class Color : WORD
    //{
    //    // 기본 색상
    //    Black = 0xFF000000,
    //    White = 0xFFFFFFFF,
    //    Red = 0xFFFF0000,
    //    Green = 0xFF00FF00,
    //    Blue = 0xFF0000FF,

    //    Yellow = 0xFFFFFF00,
    //    Cyan = 0xFF00FFFF,
    //    Magenta = 0xFFFF00FF,

    //    Gray = 0xFF808080,
    //    DarkGray = 0xFF404040,
    //    LightGray = 0xFFC0C0C0,

    //    // 추가 색상
    //    Orange = 0xFFFFA500,
    //    Pink = 0xFFFF69B4,
    //    Brown = 0xFFA52A2A,
    //    SkyBlue = 0xFF87CEEB,
    //    Lime = 0xFF32CD32,
    //    Navy = 0xFF000080,
    //    Purple = 0xFF800080,
    //    Teal = 0xFF008080,
    //    Gold = 0xFFFFD700,
    //    Silver = 0xFFC0C0C0,

    //    // 투명도
    //    Transparent = 0x00000000,
    //    HalfRed = 0x80FF0000
    //};
}