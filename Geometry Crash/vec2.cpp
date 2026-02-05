#include "vec2.h"
#include <math.h>

Vec2::Vec2(){}

Vec2::Vec2(float xin, float yin)
	:x(xin),y(yin){}

bool Vec2::operator ==(const Vec2& rhs) const
{
	return (x == rhs.x && y == rhs.y);
}

bool Vec2::operator !=(const Vec2& rhs) const
{
	return !(x == rhs.x && y == rhs.y);
}

Vec2 Vec2::operator + (const Vec2& rhs) const
{
	return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator - (const Vec2& rhs) const
{
	return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator * (const float val) const
{
	return Vec2(x * val, y * val);
}

Vec2 Vec2::operator / (const float val) const
{
	return Vec2(x / val, y / val);
}

void Vec2::operator += (const Vec2& rhs)
{
	x += rhs.x;
	y += rhs.y;
}

void Vec2::operator -= (const Vec2& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
}

void Vec2::operator *= (const float val)
{
	x *= val;
	y *= val;
}

void Vec2::operator /= (const float val)
{
	x /= val;
	y /= val;
}


float Vec2::length() const
{
	return sqrtf(x * x + y * y);
}

float Vec2::distance(const Vec2& rhs)const
{
	return (*this - rhs).length();
}

float Vec2::distSq(const Vec2& rhs) const
{
	float dx = x - rhs.x;
	float dy = y - rhs.y;
	return (dx * dx) + (dy * dy); // No sqrt here!
}

void Vec2::normalize()
{
	float L = sqrt(x * x + y * y);
	if (L != 0) // Essential check to avoid division by zero!
	{
		x /= L;
		y /= L;
	}
}


