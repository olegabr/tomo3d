//tomo3d.cpp
/*
 * IFBPJ algorithm implementation
 * (C) 2004-2005 olegabr. All rights reserved.
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
#include <transforms.h>

#include "transforms_ex.h"

namespace {
	void usage()
	{
		std::cout << "tomo3d start_angle end_angle increment images_directory\n";
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

		shadow2d exp_shadow(series);
		cout << "Loading projections..." << endl;
		exp_shadow.load_from_image(argv[argc-1]);

		normalize(exp_shadow);
		exp_shadow.save_to_image("normalized");

		align_to_gravity_center(exp_shadow);
		exp_shadow.save_to_image("aligned_to_gravity_center");

		cout << "Projections are loaded." << endl;

		tomo3d tomo(exp_shadow);
		tomo.IFBPJ(10, 1.0, 0.0001);
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
