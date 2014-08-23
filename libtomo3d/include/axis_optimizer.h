// axis_optimizer.h
/*
 * The utility class to optimize axis tilt
 * (C) 2004-2005 olegabr. All rights reserved.
*/
#ifndef LIBTOMO3D_AXIS_OPTIMIZER_H

#include <limits>
#include "angle.h"
#include "util.h"

class shadow2d;
class axis_optimizer
{
public :
	axis_optimizer(
//		shadow2d const& shExp,
		angle_t anglMax,
		angle_t anglAccuracy,
		point1d pntMax,
		point1d pntAccuracy,
		double dAccuracy,
		double dAccuracyDiff,
		int nMaxIterNumber
		) :
	m_anglMax(anglMax),
	m_anglAccuracy(anglAccuracy),
	m_pntMax(pntMax),
	m_dPrevAccuracy(std::numeric_limits<double>::max()),
	m_pntAccuracy(pntAccuracy),
	m_dAccuracy(dAccuracy),
	m_dAccuracyDiff(dAccuracyDiff),
	//m_shExp(shExp.get_angle_series(), shExp.get_size()),
	m_nMaxIterNumber(nMaxIterNumber) {}

	int operator () (shadow2d& shExp);

private :
	bool canContinue_(double dAccuracy);

	angle_t m_anglMax;
	angle_t m_anglAccuracy;

	point1d m_pntMax;
	point1d m_pntAccuracy;

	double m_dPrevAccuracy;
	double m_dAccuracy;
	double m_dAccuracyDiff;

	int m_nMaxIterNumber;
	//shadow2d m_shExp;
};

#define LIBTOMO3D_AXIS_OPTIMIZER_H
#endif //LIBTOMO3D_AXIS_OPTIMIZER_H
