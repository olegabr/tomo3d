//tomo3d.cpp
/*
 * The main class of tomo3d library
 * It provides FBPJ, IFBPJ algorithms and error calculating algorithm
 * (C) 2004-2005 olegabr. All rights reserved.
*/
#include <iostream>
#include <sstream>
//#include <iomanip>
//#include <fstream>
using std::cout; using std::endl;

#include <cmath>
using std::sqrt;

#include "transforms.h"
#include "tomo3d.h"

// shExp - an experimental shadow
tomo3d::tomo3d(shadow2d const& shExp) :
	m_psh_experimental(&shExp),
	m_sh1(shExp.get_angle_series(), shExp.get_size()),
	m_sh2(shExp.get_angle_series(), shExp.get_size()),
	m_object(
		size3d_t(
			shExp.get_size().height, shExp.get_size().height, shExp.get_size().width
		),
		shExp.get_point_size()
	),
	m_obj_diff(
		size3d_t(
			shExp.get_size().height, shExp.get_size().height, shExp.get_size().width
		),
		shExp.get_point_size()
	),
	m_accuracy(100.0),
	m_exp_sh_norm2(1.0),
	m_reproj_norm2(1.0) {}

tomo3d::value_type tomo3d::calc_betha()
{
	//m_sh1.clear();
	//project(m_obj_diff, m_sh1/*dst*/);
	project_overwrite(m_obj_diff, m_sh1/*dst*/);

	double diff_reproj_norm2 = calc_norm2_();
	cout << "diff_reproj_norm2 = " << diff_reproj_norm2 << endl;

	double obj_diff_norm2 = m_obj_diff.norm2();
	cout << "obj_diff_norm2 = " << obj_diff_norm2 << endl;

	double betha = obj_diff_norm2 / diff_reproj_norm2;
	cout << "betha = " << betha << endl;

	return /*0.5 * */ betha; //TODO: 0.5 ? проверить интерполятор и норму считать по интерполятору, а не от балды
}

void tomo3d::FBPJ()
{
	cout << "ro-filtrate experimental projections\n";
	filtered_shadow2d exp_filtered(*m_psh_experimental, m_sh1);

	cout << "calculate ro-norm of experimental projections\n";
	m_exp_sh_norm2 = *m_psh_experimental * exp_filtered;
	cout << "exp_norm = " << std::sqrt(m_exp_sh_norm2) << endl << endl;

	cout << "initial reconstruction: backprojection...\n";
	backproject_overwrite(exp_filtered, m_obj_diff);
	//backproject(exp_filtered, m_obj_diff);
	//backproject(exp_filtered, m_object);

	double betha = calc_betha();
	m_object.assign_scaled_diff(m_obj_diff, betha);

	m_object.save_to_image("result_initial");
}

tomo3d::accuracy_type tomo3d::calc_accuracy_light_()
{
	reproject_();

	/// Calc difference
	shadow2d& shdiff = m_sh1;
	shdiff.assign_difference(*m_psh_experimental);

	/// Filter diff
	m_filtered_shdiff = filtered_shadow2d(shdiff, m_sh2/*dst*/);

	return
		100 * std::sqrt((shdiff*m_filtered_shdiff)/m_exp_sh_norm2);
}

tomo3d::accuracy_type tomo3d::calc_accuracy_()
{
	reproject_();
	m_reproj_norm2 = calc_norm2_();

	/// Calc difference
	shadow2d& shdiff = m_sh1;
	shdiff.assign_difference(*m_psh_experimental);

	/// Filter diff
	m_filtered_shdiff = filtered_shadow2d(shdiff, m_sh2/*dst*/);

	return
		100 * std::sqrt((shdiff*m_filtered_shdiff)/m_exp_sh_norm2);
}

// sh - the buffer to place the reprojection
void tomo3d::reproject_()
{
	//m_sh1.clear();
	//project(m_object, m_sh1/*dst*/);
	project_overwrite(m_object, m_sh1/*dst*/);
}

shadow2d::value_type tomo3d::calc_norm2_()
{
	filtered_shadow2d filtered(m_sh1, m_sh2);
	return m_sh1 * filtered;
}

int tomo3d::IFBPJ(int nMaxIterNumber, double dAccuracy, double dAccuracyDiff)
{
	// it should be called before a call to IFBPJ
	FBPJ();

	cout << "iterative reconstruction refinement...\n";
	int nIteration(0);
	bool bCalcLastError = true;
	for (; nIteration < nMaxIterNumber; ++nIteration)
	{	cout << "Iteration N:\t" << nIteration << "\n"
			<< "calculate reprojections...\n";

		tomo3d::accuracy_type accuracy = calc_accuracy_();
		cout << "Relative accuracy after previous iteration: " << accuracy << "%" << endl;

		if (
			!check_accuracy_improvement(m_accuracy - accuracy, dAccuracyDiff) ||
			check_accuracy(accuracy, dAccuracy)
		) {
			bCalcLastError = false;
			break;
		}
		m_accuracy = accuracy;

		backproject_overwrite(m_filtered_shdiff, m_obj_diff);

		double betha = calc_betha();
		m_object.add_scaled_diff(m_obj_diff, betha);

		//double betha = std::sqrt(m_reproj_norm2/m_object.norm2());
		//cout << "betha = " << betha << endl << endl;

		//cout << "backproject the difference...\n";
		//shadow2object(m_filtered_shdiff, m_object, betha);

		save_result_(nIteration);
	}

	//calculate last error
	if (bCalcLastError) {
		m_accuracy = calc_accuracy_light_();
		cout << "Relative accuracy after last iteration: " << m_accuracy << "%" << endl;
	}

	return nIteration;
}

bool tomo3d::check_accuracy_improvement(accuracy_type accuracy_difference, accuracy_type dAccuracyDiff) const
{
	bool bResult = true;
	std::stringstream ss;

	if (accuracy_difference < 0.0) {
		ss
			<< "Accuracy improvement '" << accuracy_difference
			<< "' is negative. Stop iteration.\n";
		bResult = false;
	} else if (accuracy_difference < dAccuracyDiff) {
		ss
			<< "Accuracy improvement '" << accuracy_difference
			<< "' is less then minimum value set '" << dAccuracyDiff
			<< "'. Stop iteration.\n";
		bResult = false;
	}
	cout << ss.str();
	return bResult;
}

bool tomo3d::check_accuracy(accuracy_type accuracy, accuracy_type dAccuracy) const
{
	bool bResult = false;
	if (accuracy <= dAccuracy) {
		cout
			<< "Desired accuracy (" << dAccuracy
			<< ") is reached (" << accuracy
			<< "). Stop iteration.\n";
		bResult = true;
	}
	return bResult;
}

void tomo3d::save_result_(int nIteration) const
{
	int err = static_cast<int>(m_accuracy);
	std::stringstream ss;
	ss << nIteration << "_" << err << "_result";
	m_object.save_to_image(ss.str());
}




//#if 0
//void tomo3d::FBPJ()
//{
//	cout << "ro-filtrate experimental projections\n";
//	filtered_shadow2d exp_filtered(*m_psh_experimental, m_sh1);
//
//	cout << "calculate ro-norm of experimental projections\n";
//	m_exp_sh_norm2 = *m_psh_experimental * exp_filtered;
//	cout << "exp_norm = " << std::sqrt(m_exp_sh_norm2) << endl << endl;
//
//	cout << "initial reconstruction: backprojection...\n";
//	shadow2object(exp_filtered, m_object);
//	m_object.save_to_image("result_initial");
//}
//
//tomo3d::accuracy_type tomo3d::calc_accuracy_light_()
//{
//	reproject_();
//
//	/// Calc difference
//	shadow2d& shdiff = m_sh1;
//	shdiff.assign_difference(*m_psh_experimental);
//
//	/// Filter diff
//	m_filtered_shdiff = filtered_shadow2d(shdiff, m_sh2/*dst*/);
//
//	return
//		100 * std::sqrt((shdiff*m_filtered_shdiff)/m_exp_sh_norm2);
//}
//
//tomo3d::accuracy_type tomo3d::calc_accuracy_()
//{
//	reproject_();
//	m_reproj_norm2 = calc_norm2_();
//
//	/// Calc difference
//	shadow2d& shdiff = m_sh1;
//	shdiff.assign_difference(*m_psh_experimental);
//
//	/// Filter diff
//	m_filtered_shdiff = filtered_shadow2d(shdiff, m_sh2/*dst*/);
//
//	return
//		100 * std::sqrt((shdiff*m_filtered_shdiff)/m_exp_sh_norm2);
//}
//
//// sh - the buffer to place the reprojection
//void tomo3d::reproject_()
//{
//	m_sh1.clear();
//	object2shadow(m_object, m_sh1/*dst*/);
//}
//
//shadow2d::value_type tomo3d::calc_norm2_()
//{
//	filtered_shadow2d reproj_filtered(m_sh1, m_sh2);
//	return m_sh1 * reproj_filtered;
//}
//
//int tomo3d::IFBPJ(int nMaxIterNumber, double dAccuracy, double dAccuracyDiff)
//{
//	// it should be called before a call to IFBPJ
//	FBPJ();
//
//	cout << "iterative reconstruction refinement...\n";
//	int nIteration(0);
//	bool bCalcLastError = true;
//	for (; nIteration < nMaxIterNumber; ++nIteration)
//	{	cout << "Iteration N:\t" << nIteration << "\n"
//			<< "calculate reprojections...\n";
//
//		tomo3d::accuracy_type accuracy = calc_accuracy_();
//		cout << "Relative accuracy after previous iteration: " << accuracy << "%" << endl;
//
//		accuracy_type accuracy_difference =
//			m_accuracy - accuracy;
//		if (accuracy_difference < dAccuracyDiff) {
//			cout
//				<< "Accuracy improvement '" << accuracy_difference
//				<< "' is less then minimum value set '" << dAccuracyDiff
//				<< "'. Stop iteration." << endl;
//			bCalcLastError = false;
//			break;
//		}
//		m_accuracy = accuracy;
//
//		if (accuracy <= dAccuracy) {
//			cout
//				<< "Desired accuracy (" << dAccuracy
//				<< ") is reached (" << accuracy
//				<< "). Stop iteration." << endl;
//			bCalcLastError = false;
//			break;
//		}
//
//		double betha = std::sqrt(m_reproj_norm2/m_object.norm2());
//		cout << "betha = " << betha << endl << endl;
//
//		cout << "backproject the difference...\n";
//		shadow2object(m_filtered_shdiff, m_object, betha);
//
//		{
//			int err = static_cast<int>(m_accuracy);
//			std::stringstream ss;
//			ss << nIteration << "_" << err << "_result";
//			m_object.save_to_image(ss.str());
//		}
//	}
//
//	//calculate last error
//	if (bCalcLastError) {
//		m_accuracy = calc_accuracy_light_();
//		cout << "Relative accuracy after last iteration: " << m_accuracy << "%" << endl;
//	}
//
//	return nIteration;
//}
//#endif
