//projection1d.cpp
/*
* the projection1d class impl
* (C) 2004 olegabr. All rights reserved.
*/
#include "projection1d.h"
#include "ro_filtrate.h"

#include <iostream>
using std::cout; using std::endl;

#include <algorithm>
#include <numeric>
#include <cmath>
#include <cassert>

#include <boost/lambda/lambda.hpp>

projection1d::projection1d(size1d_t size, point1d point_size)
	: size_(size), point_size_(point_size)
{
	//cout << "projection1d(size)" << endl;
	data_.reserve(size);
	std::fill_n
		(
			std::back_inserter(data_),
			size,
			0.
		);

	//data_.resize(size_);
	//clear();
}

projection1d::projection1d(const projection1d& other)
	: size_(other.size_), point_size_(other.point_size_)
{
	//cout << "projection1d(projection1d)" << endl;
	if (&other != this)
	{
		if (data_.size() < other.data_.size())
		{
			//cout << "projection1d(projection1d): data_.resize\n";
				//<< "data_.size() = " << data_.size() << endl
				//<< "other.data_.size() = " << other.data_.size() << endl;
			data_.resize(size_);
		}
		std::copy(other.data_.begin(), other.data_.end(), data_.begin());
	}
}
projection1d& projection1d::operator =(const projection1d& other)
{
	//cout << "projection1d::operator =(const projection1d& other)" << endl;
	if (&other != this)
	{
		point_size_ = other.point_size_;
		size_ = other.size_;

		if (data_.size() < other.data_.size())
		{
			//cout << "projection1d::operator =(projection1d): data_.resize\n";
				//<< "data_.size() = " << data_.size() << endl
				//<< "other.data_.size() = " << other.data_.size() << endl;
			data_.resize(size_);
		}
		std::copy(other.data_.begin(), other.data_.end(), data_.begin());
	}
	return *this;
}

//g = f-g
projection1d& projection1d::assign_difference(const projection1d& f_)
{
	assert(f_.point_size_ == point_size_);

	projection1d::data_type::difference_type
		w = (f_.size_ - size_)/2;
	projection1d::iterator
		p  =    data_.begin(),  e =    data_.end();
	projection1d::const_iterator
		fp = f_.data_.begin(), fe = f_.data_.end();
	if (w > 0)      std::advance(fp,  w);
	else if (w < 0) std::advance( p, -w);
	for(; p != e && fp != fe; ++p, ++fp) *p = *fp - *p;

	return *this;
}

std::ostream& operator <<(std::ostream& os, const projection1d& sh)
{
	std::copy
	(
		sh.data_.begin(), sh.data_.end(),
		std::ostream_iterator<projection1d::value_type>(os, "\t")
	);
	return os;
}

void projection1d::clear()
{
	std::fill(data_.begin(), data_.end(), 0.0);
}

void projection1d::ro_filtrate()
{
	util::ro_filtrate(data_);
	if (point_size_ != 1)
		std::transform
		(
			data_.begin(), data_.end(),
			data_.begin(),
			boost::lambda::_1/point_size_
		);
}

point1d projection1d::get_point_gt(point1d l) const
{
	return get_Rmin_() + point_size_*std::ceil((l-get_Rmin_())/point_size_);
}

pixel1d projection1d::point2index_(point1d R) const
{
	return static_cast<pixel1d>(util::round((R - get_Rmin_())/point_size_));
}

void projection1d::add_value(pixel1d i, projection1d::value_type value)
{
	if (in_grid(i, size_)) data_[i] += value;
}

void projection1d::add_value(point1d R, projection1d::value_type value)
{
	add_value(point2index_(R), value);
}

void projection1d::set_value(pixel1d i, projection1d::value_type value)
{
	if (in_grid(i, size_)) data_[i] = value;
}

void projection1d::set_value(point1d R, projection1d::value_type value)
{
	set_value(point2index_(R), value);
}

projection1d::value_type projection1d::get_value(pixel1d i) const
{
	assert(in_grid(i, size_));
	return data_[i];
}

projection1d::value_type
operator *(const projection1d& sh, const projection1d& fsh)
{
	assert(sh.get_size()       == fsh.get_size()      );
	assert(sh.get_point_size() == fsh.get_point_size());

	projection1d::const_iterator
		is = sh.data_.begin(), e = sh.data_.end(), ifs = fsh.data_.begin();
	return
		(
			.5*(*is * *ifs + *sh.data_.rbegin() * *fsh.data_.rbegin()) +
			std::inner_product (++is, --e, ++ifs, 0.)
		)*sh.get_point_size();
}

std::pair<projection1d::value_type, projection1d::value_type>
projection1d::get_min_max_() const
{
	/* named return value optimization :) */
	std::pair<projection1d::value_type, projection1d::value_type>
		result(*std::min_element(data_.begin(), data_.end()), *std::max_element(data_.begin(), data_.end()));
	return result;
}
