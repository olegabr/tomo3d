//calc_error.cpp
/*
 * Calculates error of reconstruction from
 * shadow and re-shadow
 * (C) 2006 olegabr. All rights reserved.
*/

#include <Magick++.h>

#include <iostream>
using std::cout; using std::endl;

#include <cmath>
using std::sqrt;

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#include <tomo3d.h>
#include <object3d.h>
#include <shadow2d.h>
#include <filtered_shadow2d.h>

namespace {
	void usage()
	{
		std::cout << "calc_error start_angle end_angle increment src_images_directory reproj_images_directory\n";
	}

	void scaleShadow(shadow2d& shReproj, double dK)
	{
		shadow2d::iterator iterSh = shReproj.begin();
		shadow2d::iterator iterShEnd = shReproj.end();
		for (; iterSh != iterShEnd; ++iterSh) {
			projection2d::iterator iterPr2d = iterSh->begin();
			projection2d::iterator iterPr2dEnd = iterSh->end();
			for (; iterPr2d != iterPr2dEnd; ++iterPr2d) {
				projection1d::iterator iterPr1d = iterPr2d->begin();
				projection1d::iterator iterPr1dEnd = iterPr2d->end();
				for (; iterPr1d != iterPr1dEnd; ++iterPr1d) {
					*iterPr1d *= dK;
				}
			}
		}
	}

	double calc_error(shadow2d& shSrc, shadow2d& shReproj)
	{
		shadow2d shTemp(shSrc.get_angle_series(), shSrc.get_size());

		cout << "Calculate experimental projection norm...\n";
		filtered_shadow2d shFiltered(shSrc, shTemp);
		double dSrcNorm2 = shSrc * shFiltered;

		cout << "Calculate re-projection norm...\n";
		filtered_shadow2d shReprojFiltered(shReproj, shTemp);
		double dReprojNorm2 = shReproj * shReprojFiltered;

		double dK = std::sqrt(dSrcNorm2 / dReprojNorm2);

		cout << "Scale shadow...\n";
		scaleShadow(shReproj, dK);

		cout << "Calculate difference...\n";
		shadow2d& shDiff = shReproj;
		shDiff.assign_difference(shSrc);

		cout << "Filter difference...\n";
		filtered_shadow2d shDiffFilt(shDiff, shTemp);

		return
			100 * std::sqrt((shDiff * shDiffFilt) / dSrcNorm2);
	}
}

int main(int argc, char* argv[])
{
	if (argc < 6)
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

		shadow2d exp_shadow(series);
		cout << "Loading projections..." << endl;
		exp_shadow.load_from_image(argv[4]);

		shadow2d re_shadow(series);
		cout << "Loading projections..." << endl;
		re_shadow.load_from_image(argv[5]);

		if (exp_shadow.get_size() != re_shadow.get_size()) {
			cout
				<< "Experimental shadow's size '" << exp_shadow.get_size()
				<< "' is not equal to reprojected shadow's size '" << re_shadow.get_size()
				<< "'\n";
			return 1;
		}

		if (exp_shadow.get_proj_num() != re_shadow.get_proj_num()) {
			cout
				<< "Experimental shadow's projections number '" << exp_shadow.get_proj_num()
				<< "' is not equal to reprojected shadow's projections number '" << re_shadow.get_proj_num()
				<< "'\n";
			return 1;
		}

		cout << "Calculate error...\n";
		double dError = calc_error(exp_shadow, re_shadow);

		cout << "The error is '" << dError << "%'." << endl;
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
