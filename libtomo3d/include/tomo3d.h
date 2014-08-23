//tomo3d.h
/*
 * The main class of tomo3d library
 * It provides FBPJ, IFBPJ algorithms and error calculating algorithm
 * (C) 2004-2005 olegabr. All rights reserved.
*/
#ifndef LIBTOMO3D_TOMO3D_H

#include "object3d.h"
#include "shadow2d.h"
#include "filtered_shadow2d.h"

// It can be done in two ways:
// 1) 4 shadows
// 2) 3 shadows and an additional ro-filtration on each iteration step
// currently only the approach (2) is used
class tomo3d
{
public :
	typedef double value_type;
	typedef double accuracy_type;

	// shExp - an experimental shadow
	tomo3d(shadow2d const& shExp);

	object3d& get_object() {
		return m_object;
	}

	// The reprojection is invalidated
	// after call to FBPJ or IFBPJ
	shadow2d& get_reprojection() {
		reproject_();
		return m_sh1;
	}

	// Zero accuracy means absolutely perfect reconstruction
	accuracy_type get_accuracy() const { // %
		return m_accuracy;
	}

	void set_exp_shadow(shadow2d const& sh_experimental) {
		m_psh_experimental = &sh_experimental;
		m_accuracy = 100.0;
	}

	shadow2d const& get_exp_shadow() const {
		return *m_psh_experimental;
	}

	void FBPJ();
	// if dAccuracy reached, stop iteration
	// if accuracy improvement is less than dAccuracyDiff, stop iteration
	// returns a number of iterations made
	int IFBPJ(int nMaxIterNumber = 4, double dAccuracy = 1.0/*%*/, double dAccuracyDiff = 0.10/*%*/);

	shadow2d& get_sh_buff1() {
		return m_sh1;
	}
	shadow2d& get_sh_buff2() {
		return m_sh2;
	}

private :
	// sh - the buffer to place the reprojection
	void reproject_();
	shadow2d::value_type calc_norm2_();
	// calculates accuracy of current reconstruction
	// both buffers are modified
	// m_filtered_sh is assigned to sh2 after this call
	// m_reproj_norm2 is calculated in a first case
	accuracy_type calc_accuracy_();
	accuracy_type calc_accuracy_light_();

	value_type calc_betha();

	void save_result_(int nIteration) const;
	bool check_accuracy_improvement(accuracy_type accuracy_difference, accuracy_type dAccuracyDiff) const;
	bool check_accuracy(accuracy_type accuracy, accuracy_type dAccuracy) const;

	shadow2d const* m_psh_experimental;
	// The reconstructed object
	object3d m_object;
	object3d m_obj_diff;
	// Buffers for calculations
	shadow2d m_sh1;
	shadow2d m_sh2;
	// The current accuracy %
	accuracy_type m_accuracy;
	// the square of experimental shadow's norm
	value_type m_exp_sh_norm2;

	// the square of current reprojection's norm
	value_type m_reproj_norm2;

	filtered_shadow2d m_filtered_shdiff;
};

#define LIBTOMO3D_TOMO3D_H
#endif //LIBTOMO3D_TOMO3D_H
