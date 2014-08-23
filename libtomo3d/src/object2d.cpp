//object2d.cpp
/*
* the 2d object impl
* (C) 2004 olegabr. All rights reserved.
*/
#include <iostream>
using std::cout; using std::endl;

#include <numeric>
#include <limits>
#include <algorithm>

#include "object2d.h"
#include "projection1d.h"

object2d::object2d(const object2d& other)
	: size_(other.get_size()), point_size_(other.get_point_size())
{
	//cout << "object2d(object2d)" << endl;
	data_.resize(other.data_.size());
	std::copy(other.data_.begin(), other.data_.end(), data_.begin());
}

object2d::object2d(const size2d_t& size, point2d::value_type point_size)
	: size_(size), point_size_(point_size)
{
	//cout << "object2d(size,point_size)" << endl;
	//fill data with zeros
	data_.resize(size_.width*size_.height);
	clear();
}

object2d::value_type& object2d::operator ()(const pixel2d& px)
{
	assert(in_grid(px,size_));
	return data_[pixel2index_(px)];
}

object2d::value_type object2d::operator ()(const pixel2d& px) const
{
	assert(in_grid(px,size_));
	return data_[pixel2index_(px)];
}

pixel2d object2d::point2pixel(const point2d& p) const
{
	return
		pixel2d
		(
			static_cast<pixel2d::value_type>
			(
				std::floor((get_ymax_() - p.y)/get_point_size())
			),
			static_cast<pixel2d::value_type>
			(
				std::floor((p.x - get_xmin_())/get_point_size())
			)
		);
}

void object2d::clear()
{
	std::fill(data_.begin(), data_.end(), 0.0);
}

std::ostream& operator <<(std::ostream& os, const object2d& obj)
{
	for (std::size_t row(0); row < obj.size_.height; row++)
	{
		os << '\n';
		std::copy
		(
			obj.data_.begin()+row*obj.size_.width, obj.data_.begin()+(row+1)*obj.size_.width,
			std::ostream_iterator<object2d::value_type>(os, "\t")
		);
	}
	os.flush();
	return os;
}

object2d::value_type object2d::norm2() const
{
	return
		std::inner_product
		(
			data_.begin(), data_.end(),
			data_.begin(),
			0.
		)*get_point_size()*get_point_size();
}

//namespace {
//struct el_neg
//{
//	typedef object2d::value_type value_type;
//	value_type operator () (value_type val)
//	{
//		return val > 0. ? val : 0.;
//	}
//};
//}
//void object2d::eliminate_negativeness()
//{
//	std::transform(data_.begin(), data_.end(), data_.begin(), el_neg());
//}

void object2d::add_scaled_diff(object2d const& diff, double betha)
{
	object2d::data_type::iterator pnt = data_.begin();
	object2d::data_type::const_iterator diff_pnt = diff.data_.begin();
	for (;pnt != data_.end() && diff_pnt != diff.data_.end(); ++pnt, ++diff_pnt) {
		*pnt += betha * *diff_pnt;
	}
}

void object2d::assign_scaled_diff(object2d const& diff, double betha)
{
	object2d::data_type::iterator pnt = data_.begin();
	object2d::data_type::const_iterator diff_pnt = diff.data_.begin();
	for (;pnt != data_.end() && diff_pnt != diff.data_.end(); ++pnt, ++diff_pnt) {
		*pnt = betha * *diff_pnt;
	}
}

point2d object2d::pixel2point_(const pixel2d& px) const
{
	return
		point2d
		(
			get_xmin_() + px.j*get_point_size(),
			get_ymax_() - px.i*get_point_size()
		);
}

pixel2d object2d::index2pixel_(std::size_t i) const
{
	pixel2d::value_type row = i/size_.width;
	return pixel2d(row, i - size_.width*row);
}

std::size_t object2d::pixel2index_(const pixel2d& px) const
{
	return px.i*size_.width + px.j;
}

bool operator != (const object2d::iterator& lh, const object2d::iterator& rh)
{
	return lh.index_ != rh.index_;
}

bool operator != (const object2d::const_iterator& lh, const object2d::const_iterator& rh)
{
	return lh.index_ != rh.index_;
}

std::pair<object2d::value_type, object2d::value_type>
object2d::get_min_max_() const
{
	/* named return value optimization :) */
	std::pair<object2d::value_type, object2d::value_type>
		result(*std::min_element(data_.begin(), data_.end()), *std::max_element(data_.begin(), data_.end()));
	return result;
}
