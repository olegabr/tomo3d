//angle.cpp
/*
* the angle type. it saves sin() cos() evaluation and adds some syntax sugar
* (C) 2005 olegabr. All rights reserved.
*/
#include <limits>
#include <iostream>
#include "angle.h"

namespace {
inline double check_zero(double d)
{
	return std::abs(d) <= 1.01*std::numeric_limits<double>::epsilon() ? 0.0 : d;
}
}

const double angle_t::PI = 3.1415926535897932384626433832795;

angle_t::angle_t(double grad)
{
	angle_ = grad;
	cs_ = check_zero(std::cos(angle_t::PI*grad/180));
	sn_ = check_zero(std::sin(angle_t::PI*grad/180));
}

double angle_t::to_radians() const
{
	return angle_t::PI*angle_/180;
}

angle_t& angle_t::abs()
{
	angle_ = std::abs(angle_);
	sn_ = std::abs(sn_);
	return *this;
}

double cos(const angle_t& a)
{
	return a.cos();
}

double sin(const angle_t& a)
{
	return a.sin();
}

angle_t abs(const angle_t& a)
{
	angle_t t(a);
	return t.abs();
}

angle_t& angle_t::operator +=(const angle_t& a)
{
	angle_ += a.angle_;
	cs_ = check_zero(std::cos(angle_t::PI*angle_/180));
	sn_ = check_zero(std::sin(angle_t::PI*angle_/180));
	return *this;
}

angle_t& angle_t::operator -=(const angle_t& a)
{
	angle_ -= a.angle_;
	cs_ = check_zero(std::cos(angle_t::PI*angle_/180));
	sn_ = check_zero(std::sin(angle_t::PI*angle_/180));
	return *this;
}

angle_t& angle_t::operator *=(double m)
{
	angle_ *= m;
	cs_ = check_zero(std::cos(angle_t::PI*angle_/180));
	sn_ = check_zero(std::sin(angle_t::PI*angle_/180));
	return *this;
}

angle_t angle_t::operator -() const
{
	angle_t a;
	a.angle_ = -angle_;
	a.sn_ = -sn_;
	a.cs_ = cs_;
	return a;
}

bool operator == (const angle_t& lh, const angle_t& rh)
{
	return lh.angle_ == rh.angle_;
}

bool operator < (const angle_t& lh, const angle_t& rh)
{
	return lh.angle_ < rh.angle_;
}

double operator / (const angle_t& lh, const angle_t& rh)
{
	return lh.angle_/rh.angle_;
}

std::ostream& operator <<(std::ostream& os, const angle_t& a)
{
	os << a.to_grad();
	return os;
}
