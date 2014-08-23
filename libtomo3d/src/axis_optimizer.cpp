//axis_optimizer.cpp
/*
* axis_optimizer implementation.
* (C) 2006 olegabr. All rights reserved.
*/

#include "axis_optimizer.h"
#include "axis_tilt_optimizer.h"
#include "axis_position_optimizer.h"
#include "min_finder1d.h"
#include "shadow2d.h"
#include "tomo3d.h"
#include <iostream>
#include <sstream>

bool axis_optimizer::canContinue_(double dAccuracy)
{
	double dAccuracyDiff =
		m_dPrevAccuracy - dAccuracy;

	bool bCanContinue = true;
	std::stringstream ss;

	if (dAccuracyDiff < 0.0) {
		ss
			<< "Accuracy improvement '" << dAccuracyDiff
			<< "' is negative. Stop iteration.\n";
		bCanContinue = false;
	}
	if (dAccuracyDiff < m_dAccuracyDiff) {
		ss
			<< "Accuracy improvement '" << dAccuracyDiff
			<< "' is less then minimum value set '" << m_dAccuracyDiff
			<< "'. Stop iteration.\n";
		bCanContinue = false;
	}
	m_dPrevAccuracy = dAccuracy;

	if (dAccuracy <= m_dAccuracy) {
		ss
			<< "Desired accuracy (" << m_dAccuracy
			<< ") is reached (" << m_dAccuracy
			<< "). Stop iteration.\n";
		bCanContinue = false;
	}

	std::cout << ss.str();
	return bCanContinue;
}

// returns the number of iterations performed
int axis_optimizer::operator () (shadow2d& shExp)
{
	// make a copy
	//shadow2d shExp1(shExp);
	shadow2d& shExp1 = shExp;
	shadow2d m_shExp(shExp);
	tomo3d tomo(shExp);

	// 1) axis_tilt_optimizer
	typedef min_finder1d<angle_t, tomo3d::value_type, axis_tilt_optimizer> tilt_finder_type;
	tilt_finder_type
		find_tilt(-m_anglMax, m_anglMax, 5, m_anglAccuracy);
	axis_tilt_optimizer tilt_optimizer(m_shExp, tomo);

	// 2) axis_position_optimizer
	typedef min_finder1d<point1d, tomo3d::value_type, axis_position_optimizer> position_finder_type;
	position_finder_type
		find_position(-m_pntMax, m_pntMax, 5, m_pntAccuracy);
	axis_position_optimizer position_optimizer(m_shExp, tomo);

	//int nIterationNumber = 0;

	angle_t anglOffset;
	point1d pntOffset = 0;

	// 3) loop
	std::cout << "Iterative axis refinement...\n";
	int nIteration(0);
	bool bIsContinuePositionCalc = true;
	bool bIsContinueAngleCalc = true;
	for (; nIteration < m_nMaxIterNumber; ++nIteration)
	{	std::cout << "\n\n\nIteration N:\t" << nIteration << "\n";

		if (bIsContinuePositionCalc) {
			position_optimizer.setIterationNumber(nIteration);
			position_finder_type::x2y_pair posResult =
				find_position(position_optimizer);

			position_finder_type::xtype pntImprovement = posResult.first - pntOffset;
			if (std::abs(pntImprovement) < m_pntAccuracy) {
				std::cout
					<< "\nPosition offset accuracy improvement '" << pntImprovement
					<< "' is less than minimum value set '" << m_pntAccuracy << "'.\n";
				pntOffset = posResult.first;
				std::cout << "\nPosition offset found: '" << pntOffset << "'.\n";
				bIsContinuePositionCalc = false;
			}
			pntOffset = posResult.first;
			tomo3d::value_type posAccuracy = posResult.second;
			std::cout << "\nPosition offset found: '" << pntOffset << "'.\n";
			std::cout << "Accuracy with this offset: '" << posAccuracy << "'.\n";

			//deviation_t dev(point2d(0, pntOffset));
			//shExp.assign_and_deviate(shExp1, dev);
			swap(m_shExp, shExp1);
			// now shExp1 is modified
			// and m_shExp contains initial experimental shadow

			if (!canContinue_(posAccuracy)) {
				break;
			}
		}

		if (bIsContinueAngleCalc) {
			tilt_optimizer.setIterationNumber(3/*nIteration*/);
			tilt_finder_type::x2y_pair anglResult =
				find_tilt(tilt_optimizer);
			// now m_shExp contains modified by tilt_optimizer shadow

			tilt_finder_type::xtype anglImprovement = anglResult.first - anglOffset;
			if (abs(anglImprovement) < m_anglAccuracy) {
				std::cout
					<< "\nAngle offset accuracy improvement '" << anglImprovement
					<< "' is less than minimum value set '" << m_anglAccuracy << "'.\n";
				anglOffset = anglResult.first;
				std::cout << "\nAngle offset found: '" << anglOffset << "'.\n";
				bIsContinueAngleCalc = false;
			}

			anglOffset = anglResult.first;
			tomo3d::value_type anglAccuracy = anglResult.second;
			std::cout << "\nAngle offset found: '" << anglOffset << "'.\n";
			std::cout << "Accuracy with this offset: '" << anglAccuracy << "'.\n";

			//shExp.assign_and_rotate(shExp1, anglOffset);
			swap(m_shExp, shExp1);
			// now shExp1 is modified

			if (!canContinue_(anglAccuracy)) {
				break;
			}
		}
	}

	return nIteration;
}
