//point2d.h
/*
* the point in the real continuous plane
* (C) 2005 olegabr. All rights reserved.
*/
#ifndef LIBTOMO3D_POINT2D_H
#define LIBTOMO3D_POINT2D_H

#include "util.h"
#include <iosfwd>
#include <boost/operators.hpp>

struct point2d
	: boost::additive<point2d
	, boost::multipliable<point2d, double
	, boost::equality_comparable<point2d
	> > >
{
	typedef double value_type;
	value_type x,y;
	explicit point2d(value_type x_=0, value_type y_=0) : x(x_), y(y_) {}
	point2d(const point2d& p) : x(p.x), y(p.y) {}
	point2d& operator = (const point2d& p)
	{
		x = p.x; y = p.y;
		return *this;
	}
	point2d& operator += (const point2d& p)
	{
		x += p.x; y += p.y;
		return *this;
	}
	point2d& operator -= (const point2d& p)
	{
		x -= p.x; y -= p.y;
		return *this;
	}
	point2d& operator *= (double d)
	{
		x *= d; y *= d;
		return *this;
	}
};

bool operator == (const point2d& lh, const point2d& rh);
std::ostream& operator << (std::ostream& os, const point2d& p);
point2d::value_type norm(const point2d& lh);

class angle_t;
point1d operator *(const point2d& r, const angle_t& theta);
point2d& rotate(point2d&, const angle_t&);


template <typename XType, typename YType>
struct point2d_t
	: boost::additive<point2d_t<XType, YType>
	, boost::multipliable<point2d_t<XType, YType>, double
	, boost::equality_comparable<point2d_t<XType, YType>
	> > >
{
	typedef XType xvalue_type;
	typedef YType yvalue_type;

	xvalue_type x;
	yvalue_type y;

	explicit point2d_t(xvalue_type x_=0, yvalue_type y_=0) : x(x_), y(y_) {}
	point2d_t(const point2d_t& p) : x(p.x), y(p.y) {}
	point2d_t& operator = (const point2d_t& p)
	{
		x = p.x; y = p.y;
		return *this;
	}
	point2d_t& operator += (const point2d_t& p)
	{
		x += p.x; y += p.y;
		return *this;
	}
	point2d_t& operator -= (const point2d_t& p)
	{
		x -= p.x; y -= p.y;
		return *this;
	}
	point2d_t& operator *= (double d)
	{
		x *= d; y *= d;
		return *this;
	}
};

template <typename XType, typename YType>
bool operator == (const point2d_t<XType, YType>& lh, const point2d_t<XType, YType>& rh)
{
	return lh.x == rh.x && lh.y == rh.y;
}

template <typename XType, typename YType>
std::ostream& operator << (std::ostream& os, const point2d_t<XType, YType>& p)
{
	os << '(' << p.x << ',' << p.y << ')';
	return os;
}

#endif //LIBTOMO3D_POINT2D_H
