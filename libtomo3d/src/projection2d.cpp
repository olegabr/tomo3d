//projection2d.cpp
/*
* the projection2d class impl
* (C) 2005 olegabr. All rights reserved.
*/
#include "projection2d.h"
#include "bilinear_interpolator.h"

#include <iostream>
using std::cout; using std::endl;

#include <algorithm>
#include <numeric>
#include <cmath>
#include <cassert>

void projection2d::draw_cube(double d)
{
	projection2d::data_type::iterator p1d = data_.begin();
	for(pixel2d px(0,0); p1d != data_.end(); ++p1d, ++px.j)
	{
		projection1d::data_type::iterator q = p1d->begin();
		for(px.i = 0; q != p1d->end(); ++q, ++px.i)
			if (
				px.j>=0.25*size_.width && px.j<0.75*size_.width &&
				px.i>=0.25*size_.height && px.i<0.75*size_.height
				)
				set_value_(px, d);
	}
}

projection2d::projection2d()
	: point_size_(1)
{
	//cout << "projection2d(void)" << endl;
}

projection2d::projection2d
(
	const size2d_t& size,
	point2d::value_type point_size,
	const angle_t& angle
)
	: size_(size), point_size_(point_size), angle_(angle)
{
	data_.reserve(size.width);
	//for(std::size_t i(0); i < size_.width; ++i)
	//	data_.push_back(new projection1d(size.height, point_size));
	std::fill_n
		(
			std::back_inserter(data_),
			size.width,
			/*new */projection1d(size.height, point_size)
		);
}

projection2d::~projection2d()
{
	//projection2d::data_type::iterator
	//	p = data_.begin(), end = data_.end();
	//for(; p != end; ++p) delete *p;
}


projection2d::projection2d(const projection2d& other)
	: point_size_(other.point_size_),
	  angle_(other.angle_),
	  //deviation_(other.deviation_),
	  size_(other.size_)
{
	//assert(size_ <= other.size_);
	//cout << "projection2d(projection2d)" << endl;
	if (&other != this)
	{
		if (data_.size() < other.data_.size())
		{
			//cout << "projection2d::projection2d(const projection2d& other):\ndata_.size() < other.data_.size()" << endl;
			data_.resize(other.data_.size());
		}
		copy(other.data_.begin(), other.data_.end(), data_.begin());
	}
}

//FIXME! big = small should works
projection2d& projection2d::operator =(const projection2d& other)
{
	//assert(size_ >= other.size_);
	if (&other != this)
	{
		point_size_ = other.point_size_;
		angle_ = other.angle_;
		//deviation_ = other.deviation_;
		size_ = other.size_;

		if (data_.size() < other.data_.size())
		{
			//cout << "projection2d::operator =(const projection2d& other):\ndata_.size() < other.data_.size()" << endl;
			data_.resize(other.data_.size());
		}
		copy(other.data_.begin(), other.data_.end(), data_.begin());
	}
	return *this;
}

void projection2d::swap(projection2d& pr)
{
	data_.swap(pr.data_);
	std::swap(angle_, pr.angle_);
	std::swap(point_size_, pr.point_size_);
	std::swap(size_, pr.size_);
}

void projection2d::clear()
{
	projection2d::data_type::iterator p1d = data_.begin(), end = data_.end();
	for (; p1d != end; ++p1d) (p1d)->clear();
	//deviation_ = deviation_t();
}

//void projection2d::bin_by_pow2(int n)
//{
//	size2d_t new_sz
//	(
//		std::pow(2., std::floor(std::log(size_.width)/std::log(2))+1-n),
//		std::pow(2., std::floor(std::log(size_.height)/std::log(2))+1-n)
//	);
//	projection2d new_prj(new_sz, point_size_, angle_);
//	std::size_t dw = size_.width/new_sz.width, dh = size_.height/new_sz.height;
//}

//g = f-g
projection2d& projection2d::assign_difference(const projection2d& f_)
{
	//cout << "projection2d::assign_difference" << endl;
	assert(f_.point_size_ == point_size_);
	assert(f_.angle_      == angle_     );

	projection2d::data_type::difference_type
		w = (f_.size_.width - size_.width)/2;
	projection2d::iterator
		p  =    data_.begin(),  e =    data_.end();
	projection2d::const_iterator
		fp = f_.data_.begin(), fe = f_.data_.end();
	if (w > 0)      std::advance(fp,  w);
	else if (w < 0) std::advance( p, -w);
	for(; p != e && fp != fe; ++p, ++fp) p->assign_difference(*fp);
	return *this;
}

projection2d& projection2d::assign_and_rotate(const projection2d& f, const angle_t& a)
{
	//assert(f.size_ == size_);
	assert(&f != this);
	//assert(angle_ == f.angle_);
	angle_ = f.angle_;

	point_size_ = f.point_size_;
	projection2d::iterator p1d = data_.begin(), end = data_.end();
	for(pixel2d px(0,0); p1d != end; ++p1d, ++px.j)
	{
		projection1d::iterator q = (p1d)->begin(), pend = (p1d)->end();
		for(px.i = 0; q != pend; ++q, ++px.i)
		{
			point2d p_(pixel2point_(px));
			*q = f.get_value(rotate(p_, a));
		}
	}
	return *this;
}

bool projection2d::is_integral_(const deviation_t& d) const
{
	return
		(d.magnification_factor-std::floor(d.magnification_factor)) < 0.01 &&
		(d.offset.x-std::floor(d.offset.x)) < 0.01 &&
		(d.offset.y-std::floor(d.offset.y)) < 0.01;
}

projection2d&
projection2d::assign_and_deviate(const projection2d& f, const deviation_t& d)
{
	assert(&f != this);
	//assert(angle_ == f.angle_);
	angle_ = f.angle_;
	point_size_ = f.point_size_;

	projection2d::iterator p1d = data_.begin(), end = data_.end();
	for(pixel2d px(0,0); p1d != end; ++p1d, ++px.j)
	{
		projection1d::iterator q = (p1d)->begin(), pend = (p1d)->end();
		for(px.i = 0; q != pend; ++q, ++px.i)
		{
			if (is_integral_(d))
			{
				//cout << "is_integral(d) " << is_integral_(d) << endl;
				*q = f.get_value_( f.point2pixel_(d.magnification_factor*(pixel2point_(px) + d.offset)) );
			}
			else *q = f.get_value(d.magnification_factor*(pixel2point_(px)+d.offset));
		}
	}
	return *this;
}

std::ostream& operator <<(std::ostream& os, const projection2d& sh)
{
	os
		<< "angle = " << sh.angle_ << "\n"
		/*<< "size = " << sh.size_ << "\n"*//*
		<< "deviation = " << sh.deviation_ << std::endl*/;
	//os << "angle = " << sh.angle_ << "\n";
	//std::copy
	//(
	//	sh.data_.begin(), sh.data_.end(),
	//	std::ostream_iterator<projection1d>(os, "\n")
	//);
	return os;
}

void projection2d::ro_filtrate()
{
	//cout << "projection2d::ro_filtrate" << endl;
	projection2d::data_type::iterator p = data_.begin(), end = data_.end();
	for(; p != end; ++p) (p)->ro_filtrate();
}

pixel2d projection2d::index2pixel_(std::size_t i) const
{
	pixel2d::value_type row = i/size_.width;
	return pixel2d(row, i - size_.width*row);
}

void projection2d::set_value_(const pixel2d& px, projection2d::value_type value)
{
	if (in_grid(px, size_)) data_[px.j].set_value(px.i, value);
}

//p в системе координат этой проекции
//void projection2d::add_value_(const point2d& p, projection2d::value_type value)
//{
//	add_value_(point2pixel_(p), value);
//	//int i = static_cast<int>(util::round((p.x - get_xmin_())/point_size_));
//	//if (in_grid(i,size_.width)) data_[i].add_value(p.y, value);
//}

//p во внешней системе координат
//void projection2d::add_value(const point2d& p, projection2d::value_type value)
//{
//	add_value_(p/*(1./deviation_.magnification_factor)*(p-deviation_.offset)*/, value);
//}

//namespace {
//	struct counter {
//		std::size_t count;
//		counter() : count(0) {}
//		~counter() { std::cout << "counter: " << count << endl; }
//	};
//	counter c;
//}
projection2d::value_type projection2d::get_value_(const pixel2d& px) const
{
	if (in_grid(px, size_))
		return data_[px.j].get_value(px.i);
	//++c.count;
	return 0.;
}

pixel2d projection2d::point2pixel_(const point2d& p) const
{
	return
		pixel2d
		(
			static_cast<pixel2d::value_type>
			(
				(p.y - get_ymin_())/get_point_size()
			),// - ((p.y < get_ymin_()) ? 1 : 0),
			static_cast<pixel2d::value_type>
			(
				(p.x - get_xmin_())/get_point_size()
			)// - ((p.x < get_xmin_()) ? 1 : 0)
		);
}

point2d projection2d::pixel2point_(const pixel2d& px) const
{
	return
		point2d
		(
			get_xmin_() + px.j*get_point_size(),
			get_ymin_() + px.i*get_point_size()
		);
}

//p в системе координат этой проекции
projection2d::value_type projection2d::get_value(const point2d& p) const
{
	pixel2d px = point2pixel_(p);
	///*if (in_grid(px, size_)) */return get_value_(px);
	//return 0;
		return
			bilinear_interpolator_t
			(
				get_value_(px), get_value_(px.right()),
				get_value_(px.down()), get_value_(px.down().right())
			)((1./get_point_size())*(p-pixel2point_(px)));
	 //return 0;
}

//p во внешней системе координат
//projection2d::value_type projection2d::get_value(const point2d& p) const
//{
//	return
//		get_value_
//		(
//			/*(1./deviation_.magnification_factor)*(p-deviation_.offset)*/
//			p
//		);
//}

projection1d& projection2d::get_slice(pixel1d w)
{
	assert(in_grid(w, size_.width));
	return data_[w];
}

point2d projection2d::get_mass_center() const
{
	point2d p;
	projection2d::value_type sum(0);

	projection2d::const_iterator p1d = data_.begin(), end = data_.end();
	for(pixel2d px(0,0); p1d != end; ++p1d, ++px.j)
	{
		projection1d::const_iterator q = (p1d)->begin(), pend = (p1d)->end();
		for(px.i = 0; q != pend; ++q, ++px.i)
		{
			sum += *q;
			p += *q * pixel2point_(px);
		}
	}
	return (1./sum) * p;
}

projection2d::value_type projection2d::get_mass() const
{
	projection2d::value_type sum(0);

	projection2d::const_iterator p1d = data_.begin(), end = data_.end();
	for(; p1d != end; ++p1d)
	{
		projection1d::const_iterator q = p1d->begin(), pend = p1d->end();
		for(; q != pend; ++q) sum += *q;
	}
	return sum;
}

//sh.deviation == fsh.deviation always!
projection2d::value_type operator *(const projection2d& p, const projection2d& fp)
{
	assert(p.size_       == fp.size_      );
	assert(p.point_size_ == fp.point_size_);
	assert(p.angle_      == fp.angle_     );

	projection2d::const_iterator
		ip = p.data_.begin(), e = p.data_.end(),
		ifp = fp.data_.begin();
	return
		(
			.5*(*ip * *ifp + *p.data_.rbegin() * *fp.data_.rbegin()) +
			std::inner_product(++ip, --e, ++ifp, 0.)
		)*p.get_point_size();
}

std::pair<projection2d::value_type, projection2d::value_type>
projection2d::get_min_max_() const
{
	std::pair<projection2d::value_type, projection2d::value_type> result, tmp;
	projection2d::const_iterator i = begin(), e = end();
	for (; i != e; ++i)
	{
		std::pair<projection2d::value_type, projection2d::value_type>
			tmp(i->get_min_max_());
		result.first = result.first<tmp.first ? result.first : tmp.first;
		result.second = result.second>tmp.second ? result.second : tmp.second;
	}
	return result;
}
