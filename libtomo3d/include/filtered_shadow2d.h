//filtered_shadow2d.h
/*
* the filtered shadow2d class wrapper
* (C) 2005 olegabr. All rights reserved.
*/
#ifndef LIBTOMO3D_FILTERED_SHADOW2D_H
#define LIBTOMO3D_FILTERED_SHADOW2D_H

#include "shadow2d.h"
#include <cassert>

/*
 тень это набор проекций обьекта под разными углами.
 objects of this class can be backprojected
 and multiplied with non-filtered shadow2d
 filtering to supplied data is applied inplace
*/
class filtered_shadow2d
{
public :
	typedef shadow2d::value_type value_type;
	typedef shadow2d::data_type data_type;

	filtered_shadow2d() :
		pshadow2d_(0) {}

	//performs inplace filtering
	explicit filtered_shadow2d(shadow2d& sh) :
		pshadow2d_(&sh) {
		sh.ro_filtrate();
	}

	//performs inplace filtering
	filtered_shadow2d(shadow2d const& src, shadow2d& buff) :
		pshadow2d_(&buff) {
		buff = src;
		buff.ro_filtrate();
	}

	angle_t get_increment() const {
		assert(0 != pshadow2d_);
		return pshadow2d_->get_increment();
	}

	typedef data_type::iterator iterator;
	iterator begin() {
		assert(0 != pshadow2d_);
		return pshadow2d_->begin();
	}
	iterator end () {
		assert(0 != pshadow2d_);
		return pshadow2d_->end();
	}

	typedef data_type::const_iterator const_iterator;
	const_iterator begin() const {
		assert(0 != pshadow2d_);
		return pshadow2d_->begin();
	}
	const_iterator end () const {
		assert(0 != pshadow2d_);
		return pshadow2d_->end();
	}

	// the dot product of two shadows
	friend value_type
		operator *(const filtered_shadow2d& fsh, const shadow2d& sh);

	friend value_type
		operator *(const shadow2d& sh, const filtered_shadow2d& fsh);

private :
	shadow2d* pshadow2d_;
};

#endif //LIBTOMO3D_FILTERED_SHADOW2D_H
