//object3d_image.cpp
/*
* the 3d object image related features impl
* load/save from/to raw 3D image format
* x fastest, 1 byte per pixel
* (C) 2005 olegabr. All rights reserved.
*/
#include <limits>
#include <sstream>
#include <iomanip>
#include "object3d.h"

#include <boost/filesystem/operations.hpp>

#include <iostream>
using std::cout; using std::endl;

//debug
object3d::object3d()
//	: size_(size3d_t(16, 16, 16)), point_size_(1)
{
#if 0
	//cout << "object3d()" << endl;
	data_.reserve(size_.depth);
	std::fill_n
		(
			std::back_inserter(data_),
			size_.depth,
			object2d(size2d_t(size_.width, size_.height), point_size_)
		);

	//assume that physical dims of this cube are 2,2,2
	//zero at the center and 1 in each direction

	//iterate through 2d slices
	for (object3d::iterator slice = this->begin(); slice != this->end(); ++slice)
	{
		point3d::value_type z = slice.get_position();
		//iterate along the slice
		for (object2d::iterator o = slice->begin(); o != slice->end(); ++o)
		{
			point2d p = o.get_point();
			point3d r(2*p.x/size_.width, 2*p.y/size_.height, 2*z/size_.depth);
			//1 the main ellipse:
			{
				double a(.6), b(a), c(/*a*/.9);
				if (pow(r.x/a, 2.) + pow(r.y/b, 2.) + pow(r.z/c, 2.) <= 1) *o = 0.5;
			}
			//2 eyes:
			{
				double R(.1);
				point3d r1(-std::sqrt(.1), std::sqrt(.1), .6), r2(std::sqrt(.1), std::sqrt(.1), r1.z);
				if (pow(r.x-r1.x, 2.)+pow(r.y-r1.y, 2.)+pow(r.z-r1.z, 2.) <= pow(R, 2.)) *o = 1;
				if (pow(r.x-r2.x, 2.)+pow(r.y-r2.y, 2.)+pow(r.z-r2.z, 2.) <= pow(R, 2.)) *o = 1;
			}
			//3 zero cube near center
			{
				double a(.3);
				if (r.x>=0 && r.x<=a && r.y<=0 && r.y>=-a && r.z>=0 && r.z<=a) *o = 0;
			}
			//4 nose along y-axis
			{
				double a(.1), b(0.6), c(0.3);
				if (r.x>=-a && r.x<=a && r.y>=c && r.y<=(b+c) && r.z>=-a && r.z<=a) *o = 0.2;
			}
		}
	}
#endif
}

//construct from image
object3d::object3d(const std::string& image_name)
{
	load_from_image(image_name);
}

//typedef unsigned char byte_type;
void object3d::save_to_image(const std::string& img_name) const
{
namespace fs = boost::filesystem;
	fs::path dir(img_name, fs::native);
	if (!fs::exists(dir)) fs::create_directory(dir);
	//iterate through 2d slices
	int num(0);
	const object3d& object = *this;
	for (object3d::const_iterator slice = object.begin(); slice != object.end(); ++slice, ++num)
	{
		std::stringstream ss;
		ss << std::setw(4) << std::setfill('0') << num;
		slice->save_to_image((dir/(ss.str()+".tif")).string(), get_min_max_());
	}
}

namespace fs = boost::filesystem;
void object3d::load_from_image(const std::string& dir)
{
	fs::path dir_path(dir, fs::native);
	if (!exists( dir_path ))
		throw std::runtime_error(std::string("The directory ") + dir + " does't exists.");
	fs::directory_iterator itr(dir_path), end_itr;
	for (; itr != end_itr; ++itr)
		if ( !is_directory( *itr ) )
		{
			object2d o;
			o.load_from_image(fs::complete(*itr).native_file_string());
			data_.push_back(o);
		}
	point_size_ = 1;
	size_.depth = data_.size();
	size2d_t sz(data_.front().get_size());
	size_.width = sz.width;
	size_.height = sz.height;
}
