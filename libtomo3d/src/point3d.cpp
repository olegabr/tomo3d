//point3d.cpp
/*
* the point in the real continuous space
* (C) 2005 olegabr. All rights reserved.
*/
#include "point3d.h"
#include <cmath>
#include <iostream>

std::ostream& operator << (std::ostream& os, const point3d& p)
{
	os << '(' << p.x << ',' << p.y << ',' << p.z << ')';
	return os;
}

point3d::value_type norm(const point3d& p)
{
	return std::sqrt(p.x*p.x + p.y*p.y + p.z*p.z);
}

point3d::value_type operator * (const point3d& lh, const point3d& rh)
{
	return lh.x*rh.x + lh.y*rh.y + lh.z*rh.z;
}
