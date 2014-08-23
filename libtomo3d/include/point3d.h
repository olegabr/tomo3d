//point3d.h
/*
* the point in the real continuous space
* (C) 2005 olegabr. All rights reserved.
*/
#ifndef LIBTOMO3D_POINT3D_H
#define LIBTOMO3D_POINT3D_H

#include <boost/operators.hpp>

struct point3d
	: boost::additive<point3d
	, boost::multipliable<point3d, double
	, boost::equality_comparable<point3d
	> > >
{
	typedef double value_type;
	value_type x,y,z;

	explicit point3d(value_type x_=0, value_type y_=0, value_type z_=0) : x(x_), y(y_), z(z_) {}
	point3d(const point3d& p) : x(p.x), y(p.y), z(p.z) {}
	point3d& operator = (const point3d& p)
	{
		x = p.x; y = p.y; z = p.z;
		return *this;
	}

	point3d& operator += (const point3d& p)
	{
		x += p.x; y += p.y; z += p.z;
		return *this;
	}
	point3d& operator -= (const point3d& p)
	{
		x -= p.x; y -= p.y; z -= p.z;
		return *this;
	}
	point3d& operator *= (double d)
	{
		x *= d; y *= d; z *= d;
		return *this;
	}

	friend bool operator == (point3d const& lh, point3d const& rh) {
		return
			lh.x == rh.x &&
			lh.y == rh.y &&
			lh.z == rh.z;
	}
};

std::ostream& operator << (std::ostream& os, const point3d& p);
point3d::value_type norm(const point3d& lh);
point3d::value_type operator * (const point3d& lh, const point3d& rh);

#endif //LIBTOMO3D_POINT3D_H
