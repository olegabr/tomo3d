//size2d.h
/*
* the 2D size
* (C) 2005 olegabr. All rights reserved.
*/
#ifndef LIBTOMO3D_SIZE2D_H
#define LIBTOMO3D_SIZE2D_H

#include <iosfwd>
#include <boost/operators.hpp>

struct size2d_t
	: boost::equality_comparable<size2d_t>
{
	typedef unsigned int value_type;
	value_type width, height;

	explicit size2d_t(value_type width_ = 0, value_type height_ = 0)
		: width(width_), height(height_) {}
	size2d_t(const size2d_t& p) : width(p.width), height(p.height) {}
	size2d_t& operator = (const size2d_t& s)
	{
		width  = s.width;
		height = s.height;
		return *this;
	}
};

bool operator == (const size2d_t& lh, const size2d_t& rh);
std::ostream& operator << (std::ostream& os, const size2d_t& sz);

class angle_t;
size2d_t rotate(const size2d_t&, const angle_t&);
size2d_t::value_type norm(const size2d_t&);

#endif //LIBTOMO3D_SIZE2D_H
