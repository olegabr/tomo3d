//object2d_image.cpp
/*
* the 2d object image related features impl
* (C) 2004 olegabr. All rights reserved.
*/
#include <Magick++.h>
#include <boost/lambda/lambda.hpp>
#include <limits>
#include "object2d.h"

#include <iostream>
using std::cout; using std::endl;

//debug
object2d::object2d() : point_size_(1)
{
	size_ = size2d_t(32, 32);
	data_.resize(size_.width*size_.height);

	for (std::size_t i(0); i < size_.height*size_.width; ++i)
	{
		point2d p = pixel2point_(index2pixel_(i));
		data_[i] = 1;//p.x*p.x + p.y*p.y <= 160 ? 1 : 0;
	}
}

//construct from image
object2d::object2d(std::string image_name) : point_size_(1)
{
	//cout << "object2d(image_name)" << endl;
	Magick::Image image;
	image.read(image_name);
	size_ = size2d_t(image.columns(), image.rows());
	//copy data to object
	Magick::PixelPacket* pxls = image.getPixels(0, 0, image.columns(), image.rows());
	data_.resize(size_.width*size_.height);

	for (std::size_t i(0); i < size_.height*size_.width; ++i)
	{
		Magick::PixelPacket* pxl = pxls + i;
		data_[i] = (pxl->blue + pxl->green + pxl->red);
	}

	//normalize values in order to reduce the chance for numerical overflow
	value_type max_val = 3*(std::numeric_limits<Magick::Quantum>::max() - 1);
	std::transform
	(
		data_.begin(), data_.end(),
		data_.begin(),
		boost::lambda::_1/max_val
	);
}

void object2d::save_to_image(std::string img_name, std::pair<value_type, value_type> const &min_max) const
{
	Magick::Image img;
	img.size(Magick::Geometry(size_.width, size_.height));
	Magick::PixelPacket* pxls = img.getPixels(0, 0, size_.width, size_.height);

	value_type max_val(min_max.second), min_val(min_max.first);

	for (std::size_t i(0); i < size_.height*size_.width; ++i)
	{
		Magick::PixelPacket* pxl = pxls + i;
		Magick::Quantum color =
			static_cast<Magick::Quantum>
			(
				(
					(data_[i] - min_val) *
					static_cast<double>(std::numeric_limits<Magick::Quantum>::max() - 1)
				) / (max_val - min_val + 1)
			);
		pxl->blue = color;
		pxl->red = color;
		pxl->green = color;
	}

	img.syncPixels();
	img.type(Magick::GrayscaleType);
	img.write(img_name);
}

void object2d::load_from_image(const std::string& img_name)
{
	Magick::Image img;
	img.read(img_name);

	size_ = size2d_t(img.columns(), img.rows());
	point_size_ = 1;

	const std::size_t n = norm(size_);
	data_.resize(n);

	Magick::PixelPacket* pxl = img.getPixels(0, 0, size_.width, size_.height);
	for (std::size_t i(0); i < n; ++i, pxl += 1)
		data_[i] = (1./3.)*(pxl->red + pxl->green + pxl->blue);
}
