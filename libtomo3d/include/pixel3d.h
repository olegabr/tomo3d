//pixel3d.h
/*
* the point in the 3d discrete grid
* (C) 2005 olegabr. All rights reserved.
*/
#ifndef LIBTOMO3D_PIXEL3D_H
#define LIBTOMO3D_PIXEL3D_H

#include <iosfwd>
#include <boost/operators.hpp>

// i x j x k
struct pixel3d
	: boost::equality_comparable<pixel3d
	, boost::addable<pixel3d> >
{
	typedef /*unsigned*/ int value_type;
	value_type
		i, //row
		j, //column
		k; //depth
	explicit pixel3d(value_type i_=0, value_type j_=0, value_type k_=0) : i(i_), j(j_), k(k_) {}
	pixel3d(const pixel3d& p) : i(p.i), j(p.j), k(p.k) {}
	pixel3d& operator = (const pixel3d& p)
	{
		i = p.i; j = p.j; k = p.k;
		return *this;
	}
	pixel3d& operator += (const pixel3d& p)
	{
		i += p.i; j += p.j; k += p.k;
		return *this;
	}
};

std::ostream& operator << (std::ostream& os, const pixel3d& p);
bool operator == (const pixel3d& lh, const pixel3d& rh);

struct size3d_t;
/* true if the pixel is inside the grid defined by size2d object */
bool in_grid(const pixel3d&, const size3d_t&);

#endif //LIBTOMO3D_PIXEL2D_H
