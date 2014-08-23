//axis_position_optimizer.h
/*
 * The utility class to optimize axis tilt
 * (C) 2004-2005 olegabr. All rights reserved.
*/
#ifndef LIBTOMO3D_AXIS_POSITION_OPTIMIZER_H

#include "tomo3d.h"
#include <cassert>
#include <iostream>

class axis_position_optimizer
{
public :
	// the modified shadow is placed to shResult
	axis_position_optimizer(shadow2d& shResult, tomo3d& tomo) :
		m_pshExp(&tomo.get_exp_shadow()),
		m_pshResult(&shResult),
		m_ptomo(&tomo),
		//m_shExp(shExp.get_angle_series(), shExp.get_size()),
		m_nIterationNumber(0) {}

	tomo3d::value_type operator () (const point1d& axis_offset)
	{
		std::cout << "Calculate accuracy for axis_position_offset = '" << axis_offset << "'\n";
		point2d pnt(0, axis_offset);
		deviation_t dev(pnt);
		m_pshResult->assign_and_deviate(*m_pshExp, dev);

		m_ptomo->set_exp_shadow(*m_pshResult);
			m_ptomo->IFBPJ(m_nIterationNumber);
			tomo3d::value_type accuracy = m_ptomo->get_accuracy();
		m_ptomo->set_exp_shadow(*m_pshExp);

		std::cout << "Calculated accuracy is '" << accuracy << "'\n";
		return accuracy;
		//point2d pnt(0, axis_offset);
		//deviation_t dev(pnt);
		//m_shExp.assign_and_deviate(*m_pshExp, dev);

		//m_ptomo->set_exp_shadow(m_shExp);
		//	m_ptomo->IFBPJ(m_nIterationNumber);
		//	tomo3d::value_type accuracy = m_ptomo->get_accuracy();
		//m_ptomo->set_exp_shadow(*m_pshExp);

		//return accuracy;
	}

	void setIterationNumber(int nIterationNumber) {
		m_nIterationNumber = nIterationNumber;
	}

private :
	//shadow2d m_shExp;
	shadow2d const* m_pshExp;
	shadow2d* m_pshResult;
	tomo3d* m_ptomo;
	int m_nIterationNumber;
};

#define LIBTOMO3D_AXIS_TILT_OPTIMIZER_H
#endif //LIBTOMO3D_AXIS_TILT_OPTIMIZER_H
