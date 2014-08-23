//shadow2d.cpp
/*
* the shadow2d class impl
* (C) 2005 olegabr. All rights reserved.
*/
#include "shadow2d.h"
//#include "filtered_shadow2d.h"

#include <iomanip>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <cassert>

#include <iostream>
using std::cout; using std::endl;

#include <boost/filesystem/operations.hpp>

std::size_t norm(const angle_series_t& as)
{
	return
	static_cast<std::size_t>
		(
			std::ceil(
				std::abs(
					(as.end - as.start)/as.increment
				)
			)+1
		);
}

//shadow2d::shadow2d() //debug
//	: increment_(angle_t(1))
//{
//	//cout << "shadow2d(void)" << endl;
//	angle_series_t series(-89, 90, 1);
//	size2d_t prj_sz(16,16);
//	data_.reserve(norm(series));
//	for (angle_t a(series.start); a <= series.end; a += series.increment)
//	{
//		data_.push_back
//		(
//			projection2d( prj_sz, 1, a )
//		);
//		if (a == 0)
//		{
//			data_.back().draw_cube();
//		}
//	}
//}

shadow2d::shadow2d(const angle_series_t& series, size2d_t projection_size)
	: increment_(series.increment), m_angle_series(series)
{
	//cout << "shadow2d(series,projection_size)" << endl;
	data_.reserve(norm(series));
	for (angle_t a(series.start); a <= series.end; a += series.increment)
	{
		data_.push_back
		(
			/*new*/ projection2d( projection_size, 1, a )
		);
	}
}

void shadow2d::load_from_image(const std::string& dir)
{
namespace fs = boost::filesystem;
	fs::path dir_path(dir, fs::native);
	if (!exists( dir_path ))
		throw std::runtime_error(std::string("The directory ") + dir + " does't exists.");
	fs::directory_iterator itr(dir_path), end_itr;
	shadow2d::iterator p = begin(), e = end();
	for (; itr != end_itr && p != e; ++itr, ++p)
		if ( !is_directory( *itr ) ) p->load(fs::complete(*itr).native_file_string());
}

shadow2d::~shadow2d()
{
	//cout << "~shadow2d" << endl;
	//shadow2d::iterator p = begin(), e = end();
	//for(; p != e; ++p) delete *p;
}

shadow2d::shadow2d(const shadow2d& other)
	//: axis_deviation_(other.axis_deviation_)
	: increment_(other.increment_), m_angle_series(other.m_angle_series)
{
	//cout << "shadow2d(shadow2d)" << endl;
	//assert(increment_ == other.increment_);
	//assert(data_.size() == other.data_.size());
	if (&other != this)
	{
		if (data_.size() < other.data_.size())
		{
			//cout << "shadow2d(shadow2d): data_.resize" << endl;
			data_.resize(other.data_.size());
		}
		//shadow2d::iterator i = begin(), e = end();
		//shadow2d::const_iterator io = other.begin();
		//for (; i != e; ++i, ++io) *i = *io;
		copy(other.data_.begin(), other.data_.end(), data_.begin());
	}
}
shadow2d& shadow2d::operator =(const shadow2d& other)
{
	assert(increment_ == other.increment_);
	assert(data_.size() == other.data_.size());
	if (&other != this)
	{
		m_angle_series = other.m_angle_series;
		//axis_deviation_ = other.axis_deviation_;
		if (data_.size() < other.data_.size())
		{
			//cout << "shadow2d::operator =(shadow2d): data_.resize" << endl;
			data_.resize(other.data_.size());
		}
		copy(other.data_.begin(), other.data_.end(), data_.begin());
		//shadow2d::iterator i = begin(), e = end();
		//shadow2d::const_iterator io = other.begin();
		//for (; i != e; ++i, ++io) **i = **io;
	}
	return *this;
}

void swap(shadow2d& lh, shadow2d& rh)
{
	assert(lh.increment_ == rh.increment_);
	lh.data_.swap(rh.data_);
}

//g = f - g
shadow2d& shadow2d::assign_difference(const shadow2d& f)
{
	assert(data_.size() == f.data_.size());
	assert(increment_ == f.increment_);

	shadow2d::iterator i = begin(), e = end();
	shadow2d::const_iterator fi = f.begin();
	for (; i != e; ++i, ++fi) (i)->assign_difference(*fi);
	return *this;
}

shadow2d& shadow2d::assign_and_rotate(const shadow2d& sh, const angle_t& a)
{
	assert(data_.size() == sh.data_.size());
	assert(increment_ == sh.increment_);

	shadow2d::iterator i = begin(), e = end();
	shadow2d::const_iterator shi = sh.begin();
	for (; i != e; ++i, ++shi) (i)->assign_and_rotate(*shi, a);
	return *this;
}

shadow2d& shadow2d::assign_and_deviate(const shadow2d& sh, const deviation_t& d)
{
	assert(data_.size() == sh.data_.size());
	assert(increment_ == sh.increment_);

	shadow2d::iterator i = begin(), e = end();
	shadow2d::const_iterator shi = sh.begin();
	for (; i != e; ++i, ++shi) (i)->assign_and_deviate(*shi, d);
	return *this;
}

std::pair<shadow2d::value_type, shadow2d::value_type>
shadow2d::get_min_max_() const
{
	std::pair<shadow2d::value_type, shadow2d::value_type> result, tmp;
	shadow2d::const_iterator i = begin(), e = end();
	for (; i != e; ++i)
	{
		std::pair<shadow2d::value_type, shadow2d::value_type>
			tmp(i->get_min_max_());
		result.first = result.first<tmp.first ? result.first : tmp.first;
		result.second = result.second>tmp.second ? result.second : tmp.second;
	}
	return result;
}


std::ostream& operator <<(std::ostream& os, const shadow2d& sh)
{
	//os
	//	//<< "axis deviation: " << sh.axis_deviation_ << '\n'
	//	<< "first projection:\n" << sh.data_.front()/* << '\n'
	//	<< "last projection:\n" << sh.data_.back()*/ << std::endl;
	std::copy
	(
		sh.data_.begin(), sh.data_.end(),
		std::ostream_iterator<projection2d>(os, "\n")
	);
	return os;
}

void shadow2d::clear()
{
	//axis_deviation_ = angle_t();
	shadow2d::iterator i = begin(), e = end();
	for (; i != e; ++i) (i)->clear();
}

namespace fs = boost::filesystem;
void shadow2d::save_to_image(std::string img_name) const
{
	fs::path dir(img_name, fs::native);
	if (!fs::exists(dir)) fs::create_directory(dir);
	//iterate through 2d slices
	shadow2d::const_iterator i = begin(), e = end();
	for (int n(0); i != e; ++i, ++n)
	{
		std::stringstream ss;
		ss << std::setw(4) << std::setfill('0') << n;
		i->save_to_image((dir/(ss.str()+".tif")).string(), get_min_max_());
	}
}

point2d::value_type shadow2d::get_point_size() const
{
	return data_.front().get_point_size();
}

size2d_t shadow2d::get_size() const
{
	return data_.front().get_size();
}

//returns old deviation value
//angle_t shadow2d::add_deviation(angle_t deviation)
//{
//	angle_t old(axis_deviation_);
//	axis_deviation_ += deviation;
//	//shadow2d::iterator i = begin(), e = end();
//	//for (; i != e; ++i) (i)->add_deviation(deviation_t(point2d(), deviation));
//	return old;
//}
//
////returns old deviation value
//angle_t shadow2d::set_deviation(angle_t deviation)
//{
//	std::swap(axis_deviation_, deviation);
//	//shadow2d::iterator i = begin(), e = end();
//	//for (; i != e; ++i) (i)->set_deviation(deviation_t(point2d(), axis_deviation_));
//	return deviation;
//}

void shadow2d::ro_filtrate()
{
	shadow2d::iterator i = begin(), e = end();
	for (; i != e; ++i) (i)->ro_filtrate();
}

//shadow2d::value_type shadow2d::norm2() const
//{
//	shadow2d sh(*this);
//	filtered_shadow2d fsh(*this, sh);
//	return sh * fsh;
//}

//shadow2d::value_type operator *(const shadow2d& sh, const shadow2d& fsh)
//{
//	assert(sh.data_.size() == fsh.data_.size());
//	assert(sh.increment_ == fsh.increment_);
//
//	shadow2d::const_iterator
//		ish = sh.data_.begin(), e = sh.data_.end()-1,
//		ifsh = fsh.data_.begin();
//	return
//		(
//			0.5*(*ish * *ifsh + *sh.data_.rbegin() * *fsh.data_.rbegin()) +
//			std::inner_product(++ish, e, ++ifsh, 0.)
//		)*sh.increment_.to_radians();
//}

//bool operator != (const shadow2d::iterator& lh, const shadow2d::iterator& rh)
//{
//	return lh.iprojection_ != rh.iprojection_;
//}
//
//bool operator != (const shadow2d::const_iterator& lh, const shadow2d::const_iterator& rh)
//{
//	return lh.iprojection_ != rh.iprojection_;
//}
