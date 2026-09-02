#include <Math/Vector2.h>

#include <Windows.h>
#include <cassert>
#include <cmath>

namespace Craft
{
	// 전역 변수 초기화.
	Vector2I Vector2I::Zero(0, 0);
	Vector2I Vector2I::One(1, 1);
	Vector2I Vector2I::Right(1, 0);
	Vector2I Vector2I::Up(0, -1);


	Vector2I::Vector2I(int x, int y)
		: x(x), y(y)
	{

	}

	Vector2I::operator COORD() const
	{
		COORD coord = {};
		coord.X = static_cast<short>(x);
		coord.Y = static_cast<short>(y);

		return coord;
	}

	Vector2I::operator COORD()
	{
		COORD coord = {};
		coord.X = static_cast<short>(x);
		coord.Y = static_cast<short>(y);

		return coord;
	}

	Vector2I Vector2I::operator+(const Vector2I& other) const
	{
		return Vector2I(x + other.x, y + other.y);
	}

	Vector2I Vector2I::operator-(const Vector2I& other) const
	{
		return Vector2I(x - other.x, y - other.y);
	}

	Vector2I Vector2I::operator*(const Vector2I& other) const
	{
		return Vector2I(x * other.x, y * other.y);
	}

	Vector2I Vector2I::operator*(int value) const
	{
		return Vector2I(x * value, y * value);
	}

	Vector2I Vector2I::operator/(const Vector2I& other) const
	{
		// TODO: 부족하다 싶으면 더 추가하라
		// x, y가 0인지 체크
		assert(other.x != 0);
		assert(other.y != 0);

		return Vector2I(x / other.x, y / other.y);
	}

	Vector2I& Vector2I::operator=(const Vector2I& other)
	{
		x = other.x;
		y = other.y;

		return *this;
	}

	bool Vector2I::operator==(const Vector2I& other) const
	{
		return (x == other.x) && (y == other.y);
	}

	bool Vector2I::operator!=(const Vector2I& other) const
	{
		//return (x != other.x) && (y != other.y);
		return !(*this == other);
	}

	/// <summary>
	/// Float Vector2
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// 
	Vector2F Vector2F::Zero(0.f, 0.f);
	Vector2F Vector2F::One(1.f, 1.f);
	Vector2F Vector2F::Right(1.f, 0.f);
	Vector2F Vector2F::Up(0.f, -1.f);

	Vector2F::Vector2F(float x, float y)
		: x(x), y(y)
	{

	}

	Vector2F::operator COORD() const
	{
		COORD coord = {};
		coord.X = static_cast<short>(x);
		coord.Y = static_cast<short>(y);

		return coord;
	}

	Vector2F::operator COORD()
	{
		COORD coord = {};
		coord.X = static_cast<short>(x);
		coord.Y = static_cast<short>(y);

		return coord;
	}

	Vector2F Vector2F::operator+(const Vector2F& other) const
	{
		return Vector2F(x + other.x, y + other.y);
	}
	Vector2F Vector2F::operator-(const Vector2F& other) const
	{
		return Vector2F(x - other.x, y - other.y);
	}
	Vector2F Vector2F::operator*(const Vector2F& other) const
	{
		return Vector2F(x * other.x, y * other.y);
	}
	Vector2F Vector2F::operator*(float value) const
	{
		return Vector2F(x * value, y * value);
	}
	Vector2F Vector2F::operator/(const Vector2F& other) const
	{
		// TODO: 부족하다 싶으면 더 추가하라
		// x, y가 0인지 체크
		assert(other.x != 0);
		assert(other.y != 0);

		return Vector2F(x / other.x, y / other.y);
	}
	Vector2F& Vector2F::operator=(const Vector2F& other)
	{
		x = other.x;
		y = other.y;

		return *this;
	}
	bool Vector2F::operator==(const Vector2F& other) const
	{
		return (x == other.x) && (y == other.y);
	}
	bool Vector2F::operator!=(const Vector2F& other) const
	{
		return !(*this == other);
	}
	const float Vector2F::Length() const
	{
		return sqrt(x * x + y * y);
	}
	const Vector2F Vector2F::Normalize() const
	{
		assert(Length() != 0);

		return Vector2F(x / Length(), y / Length());
	}

	float GetDistanceSquared(const Vector2F& position)
	{
		const float diffX = position.x;
		const float diffY = position.y * 2.f;

		return diffX * diffX + diffY * diffY;
	}
}