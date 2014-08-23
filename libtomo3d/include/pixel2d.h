//pixel2d.h
/*
* the point in the discrete grid
* (C) 2005 olegabr. All rights reserved.
*/
#ifndef LIBTOMO3D_PIXEL2D_H
#define LIBTOMO3D_PIXEL2D_H

#include <iosfwd>
#include <boost/operators.hpp>

// i x j
struct pixel2d
	: boost::additive<pixel2d
	, boost::multipliable<pixel2d, int
	, boost::equality_comparable<pixel2d
	> > >
{
	typedef /*unsigned*/ int value_type;
	value_type
		i, //row
		j; //column
	explicit pixel2d(value_type i_=0, value_type j_=0) : i(i_), j(j_) {}
	pixel2d(const pixel2d& p) : i(p.i), j(p.j) {}
	pixel2d& operator = (const pixel2d& p)
	{
		i = p.i; j = p.j;
		return *this;
	}
	pixel2d& operator += (const pixel2d& p)
	{
		i += p.i; j += p.j;
		return *this;
	}
	pixel2d& operator *= (value_type d)
	{
		i *= d; j *= d;
		return *this;
	}
	pixel2d right() { return pixel2d(i, j+1); }
	pixel2d down()  { return pixel2d(i+1, j); }
};

std::ostream& operator << (std::ostream& os, const pixel2d& p);
std::istream& operator >> (std::istream& is, pixel2d& p);
bool operator == (const pixel2d& lh, const pixel2d& rh);

struct size2d_t;
/* true if the pixel is inside the grid defined by size2d object */
bool in_grid(const pixel2d&, const size2d_t&);

#endif //LIBTOMO3D_PIXEL2D_H
