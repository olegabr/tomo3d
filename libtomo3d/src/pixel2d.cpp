//pixel2d.cpp
/*
* the point in the discrete grid
* (C) 2005 olegabr. All rights reserved.
*/
#include "pixel2d.h"
#include "size2d.h"
#include <iostream>

std::ostream& operator << (std::ostream& os, const pixel2d& p)
{
	os << '(' << p.i << ',' << p.j << ')';
	return os;
}

std::istream& operator >> (std::istream& is, pixel2d& p)
{
	char ch;
	is >> ch >> p.i >> ch >> p.j >> ch;
	return is;
}

bool operator == (const pixel2d& lh, const pixel2d& rh)
{
	return lh.i == rh.i && lh.j == rh.j;
}

/* true if the pixel is inside the grid defined by size2d object */
bool in_grid(const pixel2d& px, const size2d_t& sz)
{
	return
		px.i>=0 &&
		px.i<static_cast<pixel2d::value_type>(sz.height) &&
		px.j>=0 &&
		px.j<static_cast<pixel2d::value_type>(sz.width);
}
