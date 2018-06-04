#pragma once
#include<math.h>

struct Point
{
	float x;
	float y;
	bool operator<(const Point& p) const
	{
		return x < p.x;
	}
	//if the function returns positive values, then it is a “left turn”
	//if the function returns 0, then it is a “no turn”
	//if the function returns negative values, then it is a “right turn”
	static float leftTurn(Point a, Point b, Point o)
	{
		return (b.x - a.x)*(o.y - a.y) - (b.y - a.y)*(o.x - a.x);
	}

	static float distance(const Point & a, const Point & b)
	{
		return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
	}
	
	static float sqrdistance(const Point & a, const Point & b)
	{
		return (a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y);
	}

};

