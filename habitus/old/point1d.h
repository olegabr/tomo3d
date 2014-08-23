//point1d.h
/*
* the 1D point
* (C) 2005 olegabr. All rights reserved.
*/
#ifndef LIBTOMO3D_POINT1D_H
#define LIBTOMO3D_POINT1D_H

#include "util.h"
#include <iosfwd>
#include <boost/operators.hpp>

template <typename Type>
struct point1d_t
	: boost::additive<point1d_t<Type>
	, boost::multipliable<point1d_t<Type>, double
	, boost::equality_comparable<point1d_t<Type>
	> > >
{
	typedef Type value_type;

	value_type value;

	explicit point1d_t(value_type val_ = 0) : value(val_) {}
	point1d_t(const point1d_t& p) : value(p.value) {}
	point1d_t& operator = (const point1d_t& p)
	{
		value = p.value;
		return *this;
	}
	point1d_t& operator += (const point1d_t& p)
	{
		value += p.value;
		return *this;
	}
	point1d_t& operator -= (const point1d_t& p)
	{
		value -= p.value;
		return *this;
	}
	point1d_t& operator *= (double d)
	{
		value *= d;
		return *this;
	}
};

template <typename Type>
bool operator == (const point1d_t<Type>& lh, const point1d_t<Type>& rh)
{
	return lh.value == rh.value;
}

template <typename Type>
std::ostream& operator << (std::ostream& os, const point1d_t<Type>& p)
{
	return os << p.value;
}

#endif //LIBTOMO3D_POINT1D_H
