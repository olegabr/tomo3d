//angle.h
/*
* the angle type. it saves sin() cos() evaluation and adds some syntax sugar
* (C) 2005 olegabr. All rights reserved.
*/
#ifndef LIBTOMO3D_ANGLE_H
#define LIBTOMO3D_ANGLE_H

#include <boost/operators.hpp>

class angle_t
	: boost::additive<angle_t
	, boost::totally_ordered<angle_t
	, boost::multipliable<angle_t, double
	> > >
{
public :
	angle_t() : angle_(0), cs_(1), sn_(0) {}
	explicit angle_t(double grad);
	angle_t(const angle_t& a) : angle_(a.angle_), cs_(a.cs_), sn_(a.sn_) {}
	angle_t& operator = (const angle_t& a)
	{
		angle_ = a.angle_;
		cs_ = a.cs_;
		sn_ = a.sn_;
		return *this;
	}
	double to_radians() const;
	double to_grad() const { return angle_; }
	double cos() const { return cs_; }
	double sin() const { return sn_; }
	angle_t& abs();
	angle_t operator -() const;
	angle_t& operator +=(const angle_t& a);
	angle_t& operator -=(const angle_t& a);
	angle_t& operator *=(double m);

	friend bool operator == (const angle_t& lh, const angle_t& rh);
	friend bool operator < (const angle_t& lh, const angle_t& rh);

	friend double operator / (const angle_t& lh, const angle_t& rh);
	friend std::ostream& operator <<(std::ostream& os, const angle_t& sh);

	static const double PI;
private :
	double angle_/*grad*/, cs_, sn_;
};

double cos(const angle_t& a);
double sin(const angle_t& a);
angle_t abs(const angle_t& a);

#endif //LIBTOMO3D_ANGLE_H
