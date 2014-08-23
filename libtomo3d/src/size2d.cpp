//size2d.cpp
/*
* the 2D size impl
* (C) 2005 olegabr. All rights reserved.
*/
#include "size2d.h"
#include <iostream>
#include <cmath>
#include "angle.h"

std::ostream& operator << (std::ostream& os, const size2d_t& sz)
{
	os << '(' << sz.width << ',' << sz.height << ')';
	return os;
}

bool operator == (const size2d_t& lh, const size2d_t& rh)
{
	return lh.height==rh.height && lh.width==rh.width;
}

size2d_t::value_type norm(const size2d_t& sz)
{
	return sz.width*sz.height;
}

size2d_t rotate(const size2d_t& sz, const angle_t& a)
{
	size2d_t
		new_sz
		(
			static_cast<size2d_t::value_type>
			(
				std::ceil(sz.height*sin(abs(a)) + sz.width*cos(a))
			),
			static_cast<size2d_t::value_type>
			(
				std::ceil(sz.height*cos(a) + sz.width*sin(abs(a)))
			)
		);
	if (sz.width%2 == 0)//even
	{
		if (new_sz.width%2 != 0)//uneven
			new_sz.width += 1;
	} else //uneven
	{
		if (new_sz.width%2 == 0)//even
			new_sz.width += 1;
	}

	if (sz.height%2 == 0)//even
	{
		if (new_sz.height%2 != 0)//uneven
			new_sz.height += 1;
	} else //uneven
	{
		if (new_sz.height%2 == 0)//even
			new_sz.height += 1;
	}

	return new_sz;
}

//bool operator < (const size2d_t& lh, const size2d_t& rh)
//{
//	return lh.height<rh.height || lh.width<rh.width;
//}
