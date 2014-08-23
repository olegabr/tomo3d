//filtered_shadow2d.cpp
/*
* the filtered shadow2d class impl
* (C) 2005 olegabr. All rights reserved.
*/
#include "filtered_shadow2d.h"

#include <numeric>
#include <cassert>

shadow2d::value_type operator *(const filtered_shadow2d& fsh, const shadow2d& sh)
{
	assert(sh.get_proj_num() == fsh.pshadow2d_->get_proj_num());
	assert(sh.get_increment() == fsh.pshadow2d_->get_increment());

	shadow2d::const_iterator
		ish = sh.begin(), e = sh.end()-1,
		ifsh = fsh.pshadow2d_->begin();
	return
		(
			0.5*(*ish * *ifsh + *(sh.end() - 1) * *(fsh.pshadow2d_->end() - 1)) +
			std::inner_product(++ish, e, ++ifsh, 0.)
		)*sh.get_increment().to_radians();
}

shadow2d::value_type operator *(const shadow2d& sh, const filtered_shadow2d& fsh)
{
	return fsh * sh;
}
