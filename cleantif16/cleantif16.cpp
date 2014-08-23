//cleantif16.cpp
/*

(C) 2004-2005 olegabr. All rights reserved.
*/

#include <Magick++.h>
#include <limits>
#include <iostream>
using std::cout; using std::endl;

#include <cmath>
#include <vector>

#include <boost/filesystem/operations.hpp>

#include "img_generator.h"
#include <min_finder1d.h>

namespace {
	void usage()
	{
		std::cout << "cleantif16 zero_image offset_image gain_image images_directory\n";
	}
	Magick::Quantum cut_negative(double d)
	{
		return static_cast<Magick::Quantum>(d>0?d:0);
	}
}

struct pxlpckt_less
{
	bool operator ()(const Magick::PixelPacket& io, const Magick::PixelPacket& g)
	{
		return io.red < g.red;
	}
};

double ftnorm2(img_generator&, double /*0..0,5*/);

class img_norm_calculator
{
public :
	typedef koefficient_type value_type;
	img_norm_calculator
	(
		PPConstIterator opxl,
		PPConstIterator gpxl,
		PPConstIterator zpxl,
		PPIterator ipxl,
		std::size_t width, std::size_t height,
		double gain_max
	)
		: opxl_(opxl), gpxl_(gpxl), zpxl_(zpxl), ipxl_(ipxl), width_(width), height_(height), gain_max_(gain_max) {}
	value_type operator ()(koefficient_type k) const
	{
		img_generator ig(opxl_, gpxl_, zpxl_, ipxl_, width_, height_, gain_max_, k);
		return ftnorm2(ig, 0.01);
	}
private :
	PPConstIterator opxl_, gpxl_, zpxl_;
	PPIterator ipxl_;
	std::size_t width_, height_;
	const double gain_max_;
};

int main(int argc, char* argv[])
{
	if (argc < 5)
	{
		usage();
		return 0;
	}
	try {
		Magick::InitializeMagick(argv[0]);
		Magick::Image zero_, offset_, gain_;
		zero_.read(argv[1]);
		offset_.read(argv[2]);
		gain_.read(argv[3]);

		Magick::Image const &zero(zero_), &offset(offset_), &gain(gain_);
		if
		(
			gain.columns() != offset.columns() ||
			gain.rows() != offset.rows() ||
			zero.columns() != offset.columns() ||
			zero.rows() != offset.rows()
		)
			throw std::runtime_error("The gain and offset or zero_image's geometries are different.");

		PPConstIterator const
			zpxl = zero_.getPixels(0, 0, zero.columns(), zero.rows()),
			gpxl = gain_.getPixels(0, 0, gain.columns(), gain.rows()),
			opxl = offset_.getPixels(0, 0, offset.columns(), offset.rows());

		const std::size_t N(offset.columns()*offset.rows());
		double gain_max = std::max_element(gpxl, gpxl+N, pxlpckt_less())->red;

		Magick::Image img;
		namespace fs = boost::filesystem;
		std::string dir_name(argv[4]);
		fs::path dir_path(dir_name, fs::native);
		if (!exists( dir_path ))
			throw std::runtime_error(std::string("The directory '") + dir_name + "' does't exists.");

		fs::path result_path(dir_path/fs::path("result"));
		if (!fs::exists(result_path)) fs::create_directory(result_path);

		const Magick::Quantum QM(std::numeric_limits<Magick::Quantum>::max() - 1);

		//image_optimizer imopt(20, 20, 0.01);
		min_finder1d<koefficient_type, image_norm_type, img_norm_calculator> imopt(-20, 20, 20, 0.01);
		fs::directory_iterator itr(dir_path), end_itr;
		for (; itr != end_itr; ++itr)
			if ( !is_directory( *itr ) )
			{
				cout << "open image file named: " << complete(*itr).native_file_string().c_str() << endl;
				img.read(complete(*itr).native_file_string().c_str());

				if (img.columns() != offset.columns() || img.rows() != offset.rows())
					throw std::runtime_error("The image and offset geometries are different.");

				PPIterator pxl = img.getPixels(0, 0, img.columns(), img.rows());
				double img_max = std::max_element(pxl, pxl+N, pxlpckt_less())->red;

				{ /* do image cleaning */
					img_norm_calculator inc(opxl, gpxl, zpxl, pxl, img.columns(), img.rows(), gain_max);
					double k(imopt(inc).second);
					PPIterator i(pxl), iend(pxl+N);
					PPConstIterator o(opxl), z(zpxl), g(gpxl);
					for (; i != iend; ++i, ++o, ++g, ++z)
					{
						i->red = i->green = i->blue =
							cut_negative(calc_img_value(i->red, z->red, k, o->red, gain_max, g->red)*(QM/img_max));
					}
				}

				img.syncPixels();
				img.type(Magick::GrayscaleType);
				img.write(complete(result_path/fs::path(std::string("clean_")+itr->leaf())).native_file_string().c_str());
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
