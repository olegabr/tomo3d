//size3d.cpp
/*
* the 3D size impl
* (C) 2005 olegabr. All rights reserved.
*/
#include "size3d.h"
#include <iostream>
#include <cmath>

std::ostream& operator << (std::ostream& os, const size3d_t& sz)
{
	os << '(' << sz.width << ',' << sz.height << ',' << sz.depth << ')';
	return os;
}

bool operator == (const size3d_t& lh, const size3d_t& rh)
{
	return lh.height==rh.height && lh.width==rh.width && lh.depth==rh.depth;
}

size3d_t::value_type norm(const size3d_t& sz)
{
	return sz.width*sz.height*sz.depth;
}
