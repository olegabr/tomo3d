//pixel3d.cpp
/*
* the point in the 3d discrete grid
* (C) 2005 olegabr. All rights reserved.
*/
#include "pixel3d.h"
#include "size3d.h"
#include <iostream>

std::ostream& operator << (std::ostream& os, const pixel3d& p)
{
	os << '(' << p.i << ',' << p.j << ',' << p.k << ')';
	return os;
}

bool operator == (const pixel3d& lh, const pixel3d& rh)
{
	return lh.i == rh.i && lh.j == rh.j && lh.k == rh.k;
}

/* true if the pixel is inside the grid defined by size2d object */
bool in_grid(const pixel3d& px, const size3d_t& sz)
{
	return
		px.i>=0 &&
		px.i<static_cast<pixel3d::value_type>(sz.height) &&
		px.j>=0 &&
		px.j<static_cast<pixel3d::value_type>(sz.width) &&
		px.k>=0 &&
		px.k<static_cast<pixel3d::value_type>(sz.depth);
}
