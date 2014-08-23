//moveaxis.cpp
/*
* It moves axis in X and/or Y directions (the axis is Z direction)
* (C) 2006 olegabr. All rights reserved.
*/

#include <Magick++.h>
#include <limits>
#include <iostream>
using std::cout; using std::endl;

#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;

#include <boost/lexical_cast.hpp>

namespace {
	void usage()
	{
		std::cout << "moveaxis xvalue yvalue start_angle increment_angle images_directory\n";
	}
}

int main(int argc, char* argv[])
{
	if (argc < 4)
	{
		usage();
		return 0;
	}
	try {
		Magick::InitializeMagick(argv[0]);

		double dXOff = boost::lexical_cast<double>(argv[1]);
		double dYOff = boost::lexical_cast<double>(argv[2]);

		double dAngleStart = 3.1415 * boost::lexical_cast<double>(argv[3]) / 180;
		double dAngleIncr = 3.1415 * boost::lexical_cast<double>(argv[4]) / 180;

		std::string dir_name(argv[argc - 1]);
		fs::path dir_path(dir_name, fs::native);
		if (!exists( dir_path ))
			throw std::runtime_error("The directory '" + dir_name + "' does't exists.");

		fs::path result_path(dir_path/fs::path("axis_moved"));
		if (!fs::exists(result_path)) fs::create_directory(result_path);

		fs::directory_iterator itr(dir_path), end_itr;
		Magick::Image img;
		double dAngle = dAngleStart;
		for (; itr != end_itr; ++itr, dAngle += dAngleIncr) {
			if ( !is_directory( *itr ) )
			{
				cout << "Open image file named: " << complete(*itr).native_file_string() << "\n";
				img.read(complete(*itr).native_file_string());

				//cout << "dAngle = " << dAngle << "\n";

				//cout << "std::cos(dAngle) = " << std::cos(dAngle) << "\n";
				//cout << "std::sin(dAngle) = " << std::sin(dAngle) << "\n";

				//cout << "dXOff * std::cos(dAngle) = " << dXOff * std::cos(dAngle) << "\n";
				//cout << "dYOff * std::sin(dAngle) = " << dYOff * std::sin(dAngle) << "\n";

				//int nXMove = static_cast<int>(dXOff * std::cos(dAngle));
				//int nYMove = static_cast<int>(dYOff * std::sin(dAngle));

				//unsigned int xmove = nXMove > 0 ? nXMove : -nXMove;
				//bool bXNegative = nXMove > 0 ? false : true;
				//unsigned int ymove = nYMove > 0 ? nYMove : -nYMove;
				//bool bYNegative = nYMove > 0 ? false : true;

				//cout << "xmove = " << xmove << "\n";
				//cout << "ymove = " << ymove << "\n";

				//Magick::Geometry geom(img.columns(), img.rows(), xmove, ymove, bXNegative, bYNegative);
				//img.transform(geom);

				Magick::DrawableAffine aff(1.0, 1.0, 1.0, 1.0, dXOff * std::cos(dAngle), dYOff * std::sin(dAngle));
				img.affineTransform(aff);

				img.type(Magick::GrayscaleType);
				img.write(complete(result_path/fs::path(std::string("moved_")+itr->leaf())).native_file_string());
			}
		}
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
