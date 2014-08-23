//size3d.h
/*
* the 3D size
* (C) 2005 olegabr. All rights reserved.
*/
#ifndef LIBTOMO3D_SIZE3D_H
#define LIBTOMO3D_SIZE3D_H

#include <iosfwd>
#include <boost/operators.hpp>

struct size3d_t
	: boost::equality_comparable<size3d_t>
{
	typedef unsigned int value_type;
	value_type width, height, depth;

	explicit size3d_t(value_type width_ = 0, value_type height_ = 0, value_type depth_ = 0)
		: width(width_), height(height_), depth(depth_) {}
	size3d_t(const size3d_t& p) : width(p.width), height(p.height), depth(p.depth) {}
	size3d_t& operator = (const size3d_t& p)
	{
		width  = p.width;
		height = p.height;
		depth  = p.depth;
		return *this;
	}
};

bool operator == (const size3d_t& lh, const size3d_t& rh);
std::ostream& operator << (std::ostream& os, const size3d_t& sz);

size3d_t::value_type norm(const size3d_t&);

#endif //LIBTOMO3D_SIZE3D_H
