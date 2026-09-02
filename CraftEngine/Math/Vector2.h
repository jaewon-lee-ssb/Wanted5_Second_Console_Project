#pragma once

#include <Core/Core.h>

#include <Windows.h>

namespace Craft
{
	// 2차원 벡터 클래스.
	class CRAFT_API Vector2I
	{
	public:
		Vector2I(int x = 0, int y = 0);
		~Vector2I() = default;

		// Windows 콘솔 좌표계로 변환하는 연산자 오버로딩.
		operator COORD() const;
		operator COORD();

		// 사칙 연산자 오버로딩
		Vector2I operator+(const Vector2I& other) const;
		Vector2I operator-(const Vector2I& other) const;
		Vector2I operator*(const Vector2I& other) const;
		Vector2I operator*(int value) const;
		Vector2I operator/(const Vector2I& other) const;

		// 대입 연산자 오버로딩
		Vector2I& operator=(const Vector2I& other);

		// 비교 연산자 오버로딩.
		bool operator==(const Vector2I& other) const;
		bool operator!=(const Vector2I& other) const;
	
		// TODO: 내적함수가 필요한 경우 추가하라.
		// 내적은 두 벡터의 사이각을 구할 때 사용한다.( 시야각에서 주로 사용 )
		// 사이각 구하는 방법 코사인역함수에 두 벡터를 내적한 값을 넣으면 된다.

		// 자주 사용할 만한 값을 전역 변수로 선언.
		static Vector2I Zero;
		static Vector2I One;
		static Vector2I Right;
		static Vector2I Up;
		
	public:
		// TODO: 나중에 float 좌표계로 바꾸고 싶으면 바꿔라
		// 좌표계 값으로 사용하기 위한 변수
		int x = 0;
		int y = 0;
	};

	// 2차원 벡터 클래스.
	class CRAFT_API Vector2F
	{
	public:
		Vector2F(float x = 0, float y = 0);
		~Vector2F() = default;

		// Windows 콘솔 좌표계로 변환하는 연산자 오버로딩.
		operator COORD() const;
		operator COORD();

		// 사칙 연산자 오버로딩
		Vector2F operator+(const Vector2F& other) const;
		Vector2F operator-(const Vector2F& other) const;
		Vector2F operator*(const Vector2F& other) const;
		Vector2F operator*(float value) const;
		Vector2F operator/(const Vector2F& other) const;

		// 대입 연산자 오버로딩
		Vector2F& operator=(const Vector2F& other);

		// 비교 연산자 오버로딩.
		bool operator==(const Vector2F& other) const;
		bool operator!=(const Vector2F& other) const;

		// TODO: 내적함수가 필요한 경우 추가하라.
		// 내적은 두 벡터의 사이각을 구할 때 사용한다.( 시야각에서 주로 사용 )
		// 사이각 구하는 방법 코사인역함수에 두 벡터를 내적한 값을 넣으면 된다.
		const float Length() const;
		const Vector2F Normalize() const;

		// 자주 사용할 만한 값을 전역 변수로 선언.
		static Vector2F Zero;
		static Vector2F One;
		static Vector2F Right;
		static Vector2F Up;

	public:
		// TODO: 나중에 float 좌표계로 바꾸고 싶으면 바꿔라
		// 좌표계 값으로 사용하기 위한 변수
		float x = 0.f;
		float y = 0.f;
	};

	CRAFT_API float GetDistanceSquared(const Vector2F& position);

}