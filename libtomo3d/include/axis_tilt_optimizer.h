//axis_tilt_optimizer.h
/*
 * The utility class to optimize axis tilt
 * (C) 2004-2005 olegabr. All rights reserved.
*/
#ifndef LIBTOMO3D_AXIS_TILT_OPTIMIZER_H

#include "tomo3d.h"
#include <cassert>
#include <iostream>

class axis_tilt_optimizer
{
public :
	axis_tilt_optimizer(shadow2d& shResult, tomo3d& tomo) :
		m_pshExp(&tomo.get_exp_shadow()),
		m_pshResult(&shResult),
		m_ptomo(&tomo),
		//m_shExp(shExp.get_angle_series(), shExp.get_size()),
		m_nIterationNumber(0) {}

	tomo3d::value_type operator() (const angle_t& axis_offset)
	{
		std::cout << "Calculate accuracy for axis_tilt_offset = '" << axis_offset << "'\n";
		//m_shExp.assign_and_rotate(*m_pshExp, axis_offset);
		m_pshResult->assign_and_rotate(*m_pshExp, axis_offset);
		//m_ptomo->set_exp_shadow(m_shExp);
		m_ptomo->set_exp_shadow(*m_pshResult);
			m_ptomo->IFBPJ(m_nIterationNumber);
			tomo3d::value_type accuracy = m_ptomo->get_accuracy();
		m_ptomo->set_exp_shadow(*m_pshExp);
		std::cout << "Calculated accuracy is '" << accuracy << "'\n";
		return accuracy;
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
