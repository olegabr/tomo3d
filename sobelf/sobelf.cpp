//sobelf.cpp
/*
* computes the sobel filter in both horizontal and vertical dimensions
* (C) 2004-2005 olegabr. All rights reserved.
*/

#include <Magick++.h>
#include <limits>
#include <iostream>
using std::cout; using std::endl;

#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;

namespace {
	void usage()
	{
		std::cout << "sobelf images_directory\n";
	}
	/*Magick::Quantum*/double cut_negative(double d)
	{
		return /*static_cast<Magick::Quantum>*/(d>0?d:-d/*0*/);
	}
}

struct pxlpckt_less
{
	bool operator ()(const Magick::PixelPacket& io, const Magick::PixelPacket& g)
	{
		return io.red < g.red;
	}
};

typedef Magick::PixelPacket* PPIterator;
typedef Magick::PixelPacket const* PPConstIterator;

class image_access
{
public :
	image_access(PPIterator Beg, int W, int H) : Beg_(Beg), W_(W), H_(H) {}
	double operator() (PPIterator pos, int i, int j)
	{
		int I((pos-Beg_)/W_), J((pos-Beg_) - I*W_);
		if (J+j < 0 || J+j >= W_) j = -j;
		if (I+i < 0 || I+i >= H_) i = -i;
		return (pos+W_*i+j)->red;
	}
private :
	PPIterator Beg_;
	int W_, H_;
};

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		usage();
		return 0;
	}
	try {
		Magick::InitializeMagick(argv[0]);

		std::string dir_name(argv[1]);
		fs::path dir_path(dir_name, fs::native);
		if (!exists( dir_path ))
			throw std::runtime_error("The directory '" + dir_name + "' does't exists.");

		fs::path result_path(dir_path/fs::path("sobel_filtered"));
		if (!fs::exists(result_path)) fs::create_directory(result_path);

		//const Magick::Quantum QM(std::numeric_limits<Magick::Quantum>::max() - 1);

		fs::directory_iterator itr(dir_path), end_itr;
		Magick::Image img;
		for (; itr != end_itr; ++itr)
			if ( !is_directory( *itr ) )
			{
				cout << "open image file named: " << complete(*itr).native_file_string() << endl;
				img.read(complete(*itr).native_file_string());
				const std::size_t N(img.columns()*img.rows());
				Magick::Image buff(img);
				PPIterator bpxl = buff.getPixels(0, 0, buff.columns(), buff.rows());

				PPIterator pxl = img.getPixels(0, 0, img.columns(), img.rows());
				image_access ia(pxl, img.columns(), img.rows());
				PPIterator i(pxl), iend(pxl+N), b(bpxl);
				for (; i != iend; ++i, ++b)
					b->red = b->green = b->blue =
					static_cast<Magick::Quantum>((2.*i->red +
						cut_negative
						(
							- ia(i, -1, -1) + ia(i, -1, 1)
							- 2*ia(i, 0, -1) + 2*ia(i, 0, 1)
							- ia(i, 1, -1)  + ia(i, 1, 1)
						) +
						cut_negative
						(
							- ia(i, -1, -1) + ia(i, 1, -1)
							- 2*ia(i, -1, 0) + 2*ia(i, 1, 0)
							- ia(i, -1, 1)  + ia(i, 1, 1)
						))*(1./4.));

				buff.syncPixels();
				//buff.normalize();
				buff.type(Magick::GrayscaleType);
				buff.write(complete(result_path/fs::path(std::string("sobeled_")+itr->leaf())).native_file_string());
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
