//3doptim.cpp
/*
 * axis position and tilt optimization
 * (C) 2004-2006 olegabr. All rights reserved.
*/

#include <Magick++.h>

#include <iostream>
using std::cout; using std::endl;

#include <algorithm>
#include <cmath>
using std::sqrt;

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#include <tomo3d.h>
#include <object3d.h>
#include <shadow2d.h>
#include <filtered_shadow2d.h>
#include <transforms.h>
#include <axis_optimizer.h>

#include "transforms_ex.h"
#include "axis_position_optimizer.h"
#include "min_finder1d.h"

namespace {
	void usage()
	{
		std::cout << "3doptim start_angle end_angle increment images_directory\n";
	}
}


int main(int argc, char* argv[])
{
	if (argc < 5)
	{
		usage();
		return 0;
	}
	try {
		Magick::InitializeMagick(argv[0]);

		angle_t
			start    (lexical_cast<double>(argv[1])),
			end      (lexical_cast<double>(argv[2])),
			increment(lexical_cast<double>(argv[3]));
		angle_series_t series(start, end, increment);

		shadow2d shExp(series);
		cout << "Loading projections..." << endl;
		shExp.load_from_image(argv[argc-1]);

		normalize(shExp);
		shExp.save_to_image("normalized");

		align_to_gravity_center(shExp);
		shExp.save_to_image("aligned_to_gravity_center");

		cout << "Projections are loaded." << endl;

		// make a copy
		//shadow2d shExp1(shExp);

		shadow2d shExp1(shExp);//shExp.get_angle_series(), shExp.get_size());

		//tomo3d tomo(shExp);

		//// 2) axis_position_optimizer
		//typedef min_finder1d<point1d, tomo3d::value_type, axis_position_optimizer> position_finder_type;
		//position_finder_type
		//	find_position(-3, 3, 5, 1);
		//axis_position_optimizer position_optimizer(shExp, shExp1, tomo);
		//position_optimizer.setIterationNumber(0);

		//position_finder_type::x2y_pair posResult =
		//	find_position(position_optimizer);

		//point1d pntOffset = posResult.first;
		//std::cout << "\nPosition offset found: '" << pntOffset << "'.\n";

		//tomo3d::value_type posAccuracy = posResult.second;
		//std::cout << "Accuracy with this offset: '" << posAccuracy << "'.\n";

		angle_t anglMax(3);
		angle_t anglAccuracy(0.1);

		point1d pntMax = 3;//shExp.get_size().height / 5;
		//pntMax = std::max(pntMax, 10.0);
		point1d pntAccuracy = 1;

		double dAccuracy = 1;
		double dAccuracyDiff = 0.00001;

		int nMaxIterNumber = 5;

		axis_optimizer opt(
			anglMax, anglAccuracy,
			pntMax, pntAccuracy,
			dAccuracy, dAccuracyDiff,
			nMaxIterNumber
		);

		opt(shExp1);

		shExp1.save_to_image(std::string("result_") + argv[argc-1]);
	}
	catch (boost::bad_lexical_cast &ex) {
		cout << ex.what() << endl;
	}
	catch (Magick::Exception &ex) {
		cout << ex.what() << endl;
	}
	catch (std::exception &e) {
		cout << e.what() << endl;
	}
	catch (...) {
		cout << "Unknown exception occured." << endl;
	}

	return 0;
}



////3doptim.cpp
///*
//
//(C) 2004-2005 olegabr. All rights reserved.
//*/
//
//// 1) загрузить картинку
//// 2) получить проекции
//// 3) испортить их (_общий сдвиг_, случайные сдвиги каждой тени, растяжение, шум!)
//// 4) алгоритм восстановления
//// 4.1) выделение областей, где будем восстанавливать // пока все сразу восстанавливаем
//// 4.2) восстановление
//// 4.3) оптимизация положения оси
//// 4.4) оптимизация положения каждой тени и т.д.
//// 4.5) восстановление всего изображения
//// 5) сохранить картинку
//#include <Magick++.h>
//
//#include <iostream>
//#include <sstream>
//#include <iomanip>
//using std::cout; using std::endl;
//
//#include <cmath>
//using std::sqrt;
//
//#include <boost/lexical_cast.hpp>
//using boost::lexical_cast;
//
//#include <object3d.h>
//#include <shadow2d.h>
//#include <transforms.h>
//#include <min_finder1d.h>
//#include "transforms_ex.h"
//
////class error_calculator
////{
////public :
////	error_calculator
////	(
////		shadow2d::value_type& exp_norm2,
////		const shadow2d& shExp,
////		object3d& reobj,
////		shadow2d& exp_sh,
////		shadow2d& exp_filtered,
////		shadow2d& reproj,
////		shadow2d& reproj_filtered
////	)
////	: exp_norm2_(exp_norm2), shExp_(shExp),
////	  reobj_(reobj), exp_sh_(exp_sh),
////	  exp_filtered_(exp_filtered), reproj_(reproj),
////	  reproj_filtered_(reproj_filtered) {}
////
////	shadow2d::value_type operator () (const angle_t&  axis_offset)
////	{
////		exp_sh_.assign_and_rotate(shExp_, axis_offset);
////
////		exp_filtered_ = exp_sh_;
////		exp_filtered_.ro_filtrate();
////
////		exp_norm2_ = exp_sh_*exp_filtered_;
////		shadow2object(exp_filtered_, reobj_);
////
////		reproj_.clear();
////		object2shadow(reobj_, reproj_);
////		reobj_.clear();
////		reproj_.assign_difference(exp_sh_);
////
////		reproj_filtered_ = reproj_;
////		reproj_filtered_.ro_filtrate();
////
////		return std::sqrt((reproj_*reproj_filtered_)/exp_norm2_);
////	}
////private :
////	shadow2d::value_type& exp_norm2_;
////	const shadow2d& shExp_;
////	object3d& reobj_;
////	shadow2d &exp_sh_, &exp_filtered_, &reproj_, &reproj_filtered_;
////};
////
//namespace {
//void usage()
//{
//	std::cout << "tomo3d start_angle end_angle increment directory_to_the_images [max angle to variate]\n";
//}
//}
//
//int main(int argc, char* argv[])
//{
//	if (argc < 5)
//	{
//		usage();
//		return 0;
//	}
//	try {
//		Magick::InitializeMagick(argv[0]);
//
//		//если относительная ошибка меньше этого значения, то можно выходить из цикла итераций
//		double precision(0.5); //%
//
//		angle_t
//			start    (lexical_cast<double>(argv[1])),
//			end      (lexical_cast<double>(argv[2])),
//			increment(lexical_cast<double>(argv[3]));
//		angle_series_t series(start, end, increment);
//
//		shadow2d shExp(series);
//		cout << "Loading projections..." << endl;
//		shExp.load_from_image(argv[4]);
//		normalize(shExp);
//		align_shadow_to_gravity_center(shExp);
//		cout << "Projections are loaded." << endl;
//
//		//{
//		//	size2d_t sz2d(shExp.get_size());
//		//	rotate(sz2d, angle_t(5));
//		//	//sz2d.height += 20;
//		//	shadow2d tmp(series, sz2d);
//		//	tmp.assign_and_rotate(shExp, angle_t(-5));
//		//	//tmp.save_to_image("tmp");
//		//	//return 0;
//		//	swap(tmp, shExp);
//		//}
//
//		//тени, реальный смысл которых будет определяться позже
//		cout << "Allocate shadows..." << endl;
//		size2d_t sz2d(shExp.get_size());
//		shadow2d sh1(series, sz2d), sh2(series, sz2d), mask(series, sz2d);
//
//		// теперь у нас есть тень => реконструируем объект
//		size3d_t::value_type s = sz2d.height;
//		object3d
//			reobj
//			(
//				size3d_t(s, s, sz2d.width),
//				shExp.get_point_size()
//			),
//			reobj1
//			(
//				size3d_t(s, s, sz2d.width),
//				shExp.get_point_size()
//			);
//
//		const int I = 500;
//
//		projection2d::value_type exp_norm2(1);
//
//		// it is an optimization of a tilt axis (angular) position
//		angle_t axis_offset(1.23);//, max_a(0.5/*45.*/);
//		size2d_t big_shsz = sz2d;//rotate(sz2d, max_a);
//		shadow2d big_sh1(series, big_shsz), big_sh2(series, big_shsz);
//
//		big_sh1.assign_and_rotate(shExp, angle_t(1.23));
//		swap(big_sh1, shExp);
//
//		error_calculator calc_error(exp_norm2, shExp, reobj, big_sh1, big_sh2, sh1, sh2);
//		//for (angle_t a(angle_t(-1)); a <= angle_t(1); a += angle_t(0.2))
//		//	cout << a << "\t" << calc_error(a) << endl;
//		//big_sh1.save_to_image("rotated");
//		//return 0;
//
//		cout << "Optimizing axis direction...\n";
//		double max_a(argc==6?lexical_cast<double>(argv[5]):4.);
//		min_finder1d<angle_t, shadow2d::value_type, error_calculator>
//			find_axis(angle_t(-max_a), angle_t(max_a), 30, angle_t(0.1));
//
//		axis_offset = find_axis(calc_error).first;
//		cout << "axis_offset found: " << axis_offset << '\n' << endl;
//		return 0;
//
//		big_sh1.assign_and_rotate(shExp, axis_offset);
//		swap(big_sh1, shExp);
////#endif
//		//cout << "ro-filtrate experimental projections\n";
//		//sh1.assign_and_rotate(shExp, angle_t(-6));
//		//swap(sh1, shExp);
//		shadow2d& exp_filtered = sh1;
//		exp_filtered = shExp;
//		exp_filtered.ro_filtrate();
//		//exp_filtered.save_to_image("filt");
//		//return 0;
//
//		cout << "calculate ro-norm of experimental projections\n";
//		/*projection2d::value_type */
//		exp_norm2 = shExp*exp_filtered;
//		cout << "exp_norm = " << std::sqrt(exp_norm2) << endl << endl;
//
//		cout << "initial reconstruction: backprojection...\n";
//		shadow2object(exp_filtered, reobj);
//		reobj.eliminate_negativeness();
//		reobj.save_to_image("result_initial");
//
//		object3d::value_type reobj_norm2 = reobj.norm2();
//		cout << "reobj_norm = " << std::sqrt(reobj_norm2) << endl << endl;
//
//		double prev_error = 1000.;
//		double betha_k = 1.;
//		int fault_count(0);
//		//reobj.save_to_image("initial_");
////#if 0
//		cout << "iterative reconstruction refinement...\n";
//		for (int i(0); i < I; ++i)
//		{	cout << "Iteration N:\t" << i << endl;
//			//cout << "calculate reprojections...\n";
//			shadow2d& reproj = sh1;
//			reproj.clear();
//			object2shadow(reobj, reproj); //1
//
//			//reproj.save_to_image("reproj");
//			//return 0;
//
//			shadow2d& reproj_filtered = sh2;
//			reproj_filtered = reproj;
//			reproj_filtered.ro_filtrate(); //3
//			shadow2d::value_type reproj_norm2 = reproj*reproj_filtered;
//			//cout << "reproj.norm = " << reproj_norm2 << endl << endl;
//
//			//apply_mask(mask, reproj);
//			//reproj.save_to_image("reproj");
//			//reobj.save_to_image("result"/*argv[argc-1]*/);
//			//return 0;
//
//			//cout << "calculate difference of reprojections and experimental projections...\n";
//			shadow2d& shdiff = reproj;
//			shdiff.assign_difference(shExp); //2
//
//			//cout << "ro-filtrate the difference...\n";
//			shadow2d& shdiff_filtered = sh2;
//			shdiff_filtered = shdiff;
//			shdiff_filtered.ro_filtrate(); //3
//			//cout << "shdiff.norm = " << std::sqrt(shdiff*shdiff_filtered) << endl << endl;
//			//shdiff_filtered.save_to_image("shdiff_filtered");
//			apply_mask(mask, shdiff_filtered);
//			//shdiff_filtered.save_to_image("shdiff_filtered_masked");
//			//return 0;
//
//			//cout << "calculate ro-norm of the difference...\n";
//			double error = 100*sqrt((shdiff*shdiff_filtered)/exp_norm2); //4
//			cout << "Relative error after previous iteration: " << error << "%" << endl;
//
//			if ((error - prev_error) <= 0.0000001)
//			{
//				fault_count = 0;
//				reobj1 = reobj;
//			}
//			else
//			{
//				if (++fault_count > 5) break;
//				betha_k /= std::pow(1.5, 2*fault_count);
//				reobj = reobj1;
//				prev_error = 1000.;
//				continue;
//			}
//
//			if (std::abs(error - prev_error) < 1.) betha_k *= 0.5;
//			if (std::abs(error - prev_error) <= 0.002) break;
//
//			prev_error = error;
//
//			if (error <= precision) //5
//			{
//				cout << "Desired accuracy is reached, stop iteration." << endl;
//				break;
//			}
//
//			//cout << "backproject the difference...\n";
//			reobj_norm2 = reobj.norm2();
//			double betha = betha_k*std::sqrt(reproj_norm2/reobj_norm2);
//			cout << "betha = " << betha << endl << endl;
//			shadow2object(shdiff_filtered, reobj, betha); //6
//			reobj.eliminate_negativeness(); //7
//			//apply_filter(reobj, center, reobj2);
//			//reobj = reobj2;
//			//cout << "obj.norm = " << std::sqrt(reobj_norm2) << endl << endl;
//
//			{
//				int err = static_cast<int>(prev_error);
//				std::stringstream ss;
//				ss << "result_" << i << "_" << err;
//				reobj.save_to_image(ss.str());
//			}
//		}
//
//		{ //calculate last error
//			shadow2d& reproj = sh1;
//			reproj.clear();
//			object2shadow(reobj, reproj); //1
//
//			//cout << "calculate difference of reprojections and experimental projections...\n";
//			shadow2d& shdiff = reproj;
//			shdiff.assign_difference(shExp); //2
//
//			//cout << "ro-filtrate the difference...\n";
//			shadow2d& shdiff_filtered = sh2;
//			shdiff_filtered = shdiff;
//			shdiff_filtered.ro_filtrate(); //3
//			apply_mask(mask, shdiff_filtered);
//
//			//cout << "calculate ro-norm of the difference...\n";
//			double error = 100*sqrt((shdiff*shdiff_filtered)/exp_norm2); //4
//			cout << "Relative error after last iteration: " << error << "%" << endl;
//		}
////#endif
//		reobj.save_to_image("result"/*argv[argc-1]*/);
//	}
//	catch (boost::bad_lexical_cast &ex) {
//		cout << ex.what() << endl;
//	}
//	catch (Magick::Exception &ex) {
//		cout << ex.what() << endl;
//	}
//	catch (std::exception &e) {
//		cout << e.what() << endl;
//	}
//	catch (...) {
//		cout << "Unknown exception occured." << endl;
//	}
//
//	return 0;
//}
