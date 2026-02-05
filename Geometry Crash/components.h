#pragma once

#include "vec2.h"
#include<SFML/Graphics.hpp>

class CTransform
{
public:
	Vec2 pos = { 0.0f,0.0f };
	Vec2 velocity = { 0.0f,0.0f };
	float angle = 0.0f;

	CTransform(const Vec2 &p,const Vec2 &v,float a)
		:pos(p),velocity(v),angle(a){ }
};

class CShape
{
public:
	sf::CircleShape circle;

	CShape(float radius, int points, const sf::Color& fill, const sf::Color& outline, float thickness)
		:circle(radius,points)
	{

		circle.setFillColor(fill);
		circle.setOutlineThickness(thickness);
		circle.setOutlineColor(outline);
		circle.setOrigin({ radius, radius });
	}
};

class CCollision
{
public:
	float radius= 0.f;

	CCollision(float r)
		:radius(r){ }
};

class CScore
{
public:
	int score = 0;
	CScore(int s)
		:score(s){ }
};

class CLifespan
{
public:
	int remaining = 0;
	int total = 0;
	CLifespan(int total)
		:remaining(total), total(total){}
};

class CInput
{
public:
	bool up = 0;
	bool down = 0;
	bool left = 0;
	bool right = 0;
	bool shoot = 0;

	CInput(){}

};

class CBoundingBox
{
public:
	Vec2 size;
	Vec2 halfSize;

	CBoundingBox(const Vec2& s)
		: size(s), halfSize(s.x / 2.0f, s.y / 2.0f) {
	}
};
