//deviation.h
/*
* the deviation class
* (C) 2005 olegabr. All rights reserved.
*/
#ifndef LIBTOMO3D_DEVIATION_H
#define LIBTOMO3D_DEVIATION_H

#include "point2d.h"
#include "angle.h"
#include <boost/operators.hpp>

struct deviation_t
	: /*boost::additive<deviation_t
	, */boost::equality_comparable<deviation_t
	>// >
{
	point2d offset;
	//angle_t rotation_angle;
	double  magnification_factor;

	explicit deviation_t(point2d of=point2d()/*, angle_t rta=angle_t()*/, double m=1)
		: offset(of),
		  //rotation_angle(rta),
		  magnification_factor(m) {}
	deviation_t(const deviation_t& other)
		: offset(other.offset),
		  //rotation_angle(other.rotation_angle),
		  magnification_factor(other.magnification_factor) {}
	deviation_t& operator = (const deviation_t& other)
	{
		offset = other.offset;
		//rotation_angle = other.rotation_angle;
		magnification_factor = other.magnification_factor;
		return *this;
	}
	//deviation_t& operator += (const deviation_t& other)
	//{
	//	offset += other.offset;
	//	//rotation_angle += other.rotation_angle;
	//	magnification_factor *= other.magnification_factor;
	//	return *this;
	//}
};

std::ostream& operator <<(std::ostream& os, const deviation_t& dev);
bool operator == (const deviation_t& lh, const deviation_t& rh);

#endif //LIBTOMO3D_DEVIATION_H
