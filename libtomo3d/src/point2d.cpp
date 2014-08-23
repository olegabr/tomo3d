//point2d.cpp
/*
* the point in the real continuous plane
* (C) 2005 olegabr. All rights reserved.
*/
#include "point2d.h"
#include "angle.h"
#include <cmath>
#include <iostream>

bool operator == (const point2d& lh, const point2d& rh)
{
	return lh.x == rh.x && lh.y == rh.y;
}

std::ostream& operator << (std::ostream& os, const point2d& p)
{
	os << '(' << p.x << ',' << p.y << ')';
	return os;
}

point2d::value_type norm(const point2d& p)
{
	return std::sqrt(p.x*p.x+p.y*p.y);
}

point1d operator *(const point2d& r, const angle_t& theta)
{
	return r.x*cos(theta) + r.y*sin(theta);
}

point2d& rotate(point2d& p, const angle_t& a)
{
	point2d r(p);
	p.x = cos(a)*r.x + sin(a)*r.y;
	p.y = cos(a)*r.y - sin(a)*r.x;
	return p;
}