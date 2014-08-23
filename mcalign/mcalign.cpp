//mcalign.cpp
/*
 * Align projections by mass-centers
 * moving all centers to the center of the picture
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
		std::cout << "mcalign start_angle end_angle increment src_images_directory dst_images_directory\n";
	}

	void mcalign(shadow2d& sh, point2d pntCenter)
	{
		shadow2d::iterator p = sh.begin();
		shadow2d::iterator e = sh.end();

		projection2d pr(p->get_size(), p->get_point_size(), p->get_angle());
		for (; p != e; ++p)
		{
			deviation_t d(pntCenter - p->get_mass_center());
			pr.assign_and_deviate(*p, d);
			pr.swap(*p);
		}
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

		shadow2d shadow(series);
		cout << "Loading projections..." << endl;
		shadow.load_from_image(argv[4]);

		size2d_t sz = shadow.get_size();
		mcalign(shadow, point2d(sz.width / 2, sz.height / 2));

		shadow.save_to_image(argv[5]);
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
