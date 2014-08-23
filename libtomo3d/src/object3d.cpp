//object3d.cpp
/*
* the 3d object impl
* (C) 2005 olegabr. All rights reserved.
*/
#include <algorithm>
#include <cassert>
#include "object3d.h"

#include <cmath>
#include <iostream>
using std::cout; using std::endl;

//object3d::object3d(const object3d& other)
//: size_(other.get_size()), point_size_(other.get_point_size())
//{
//	//cout << "object3d(object3d)" << endl;
//	data_.resize(other.data_.size());
//	std::copy(other.data_.begin(), other.data_.end(), data_.begin());
//}

object3d::~object3d()
{
	//cout << "~object3d" << endl;
}

object3d::object3d(size3d_t size, point3d::value_type point_size)
	: size_(size), point_size_(point_size)
{
	//cout << "object3d(size)" << endl;
	data_.reserve(size_.depth);
	std::fill_n
		(
			std::back_inserter(data_),
			size_.depth,
			object2d(size2d_t(size.width, size.height), point_size)
		);
}

//coping of this object is very expensive
object3d::object3d(const object3d& other)
{
	data_.resize(other.size_.depth);
	std::copy(other.data_.begin(), other.data_.end(), data_.begin());
	size_ = other.size_;
	point_size_ = other.point_size_;
}

object3d& object3d::operator =(const object3d& other)
{
	object3d temp(other);
	temp.swap(*this);
	return *this;
}

object3d::value_type& object3d::operator ()(const pixel3d& px)
{
	assert(in_grid(px,size_));
	return data_[px.k](pixel2d(px.i, px.j));
}

object3d::value_type object3d::operator ()(const pixel3d& px) const
{
	assert(in_grid(px,size_));
	return data_[px.k](pixel2d(px.i, px.j));
}

pixel3d object3d::point2pixel(const point3d& p) const
{
	pixel3d px;
	px.k =
		static_cast<pixel3d::value_type>
		(
			std::floor((p.z - get_zmin_())/get_point_size())
		);
	pixel2d px2 = data_[px.k].point2pixel(point2d(p.x, p.y));
	px.i = px2.i; px.j = px2.j;
	return px;
}

void object3d::clear()
{
	object3d::data_type::iterator slice = data_.begin(), e = data_.end();
	for (;slice != e; ++slice) slice->clear();
}

void object3d::swap(object3d& other) /*throw()*/
{
	std::swap(size_, other.size_);
	std::swap(point_size_, other.point_size_);
	data_.swap(other.data_);
}

object3d::value_type object3d::norm2() const
{
	point3d::value_type norm(0);
	object3d::data_type::const_iterator slice = data_.begin();
	for (;slice != data_.end(); ++slice) norm += slice->norm2();
	return norm*get_point_size();
}

//void object3d::eliminate_negativeness()
//{
//	object3d::data_type::iterator slice = data_.begin(), e = data_.end();
//	for (;slice != e; ++slice) slice->eliminate_negativeness();
//}

void object3d::add_scaled_diff(object3d const& diff, double betha)
{
	object3d::data_type::iterator slice = data_.begin();
	object3d::data_type::const_iterator diff_slice = diff.data_.begin();
	for (;slice != data_.end() && diff_slice != diff.data_.end(); ++slice, ++diff_slice) {
		slice->add_scaled_diff(*diff_slice, betha);
	}
}

void object3d::assign_scaled_diff(object3d const& diff, double betha)
{
	object3d::data_type::iterator slice = data_.begin();
	object3d::data_type::const_iterator diff_slice = diff.data_.begin();
	for (;slice != data_.end() && diff_slice != diff.data_.end(); ++slice, ++diff_slice) {
		slice->assign_scaled_diff(*diff_slice, betha);
	}
}

std::pair<object3d::value_type, object3d::value_type>
object3d::get_min_max_() const
{
	std::pair<object3d::value_type, object3d::value_type> result, tmp;
	object3d::const_iterator i = begin(), e = end();
	for (; i != e; ++i)
	{
		std::pair<object3d::value_type, object3d::value_type>
			tmp(i->get_min_max_());
		result.first = result.first<tmp.first ? result.first : tmp.first;
		result.second = result.second>tmp.second ? result.second : tmp.second;
	}
	return result;
}
