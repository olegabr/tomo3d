//projection2d_image.cpp
/*
* the 2d projection image related features impl
* (C) 2005 olegabr. All rights reserved.
*/
#include <Magick++.h>
#include <limits>
#include <iostream>
using std::cout; using std::endl;
#include "projection2d.h"

void projection2d::load(const std::string& img_name)
{
	Magick::Image img;
	img.read(img_name);

	size_ = size2d_t(img.columns(), img.rows());
	point_size_ = 1;

	data_.reserve(size_.width);
	std::fill_n
		(
			std::back_inserter(data_),
			size_.width,
			projection1d(size_.height, point_size_)
		);

	std::size_t n = size_.height*size_.width;
	Magick::PixelPacket* pxl = img.getPixels(0, 0, size_.width, size_.height);
	for (std::size_t i(0); i < n; ++i, pxl += 1)
		set_value_(index2pixel_(i), (1./3.)*(pxl->red + pxl->green + pxl->blue));
}

void projection2d::save_to_image(std::string img_name, std::pair<value_type, value_type> const &min_max) const
{
	Magick::Image img;
	img.size(Magick::Geometry(size_.width, size_.height));
	Magick::PixelPacket* pxls = img.getPixels(0, 0, size_.width, size_.height);

	projection2d::value_type max_val(min_max.second), min_val(min_max.first);
	for (std::size_t i(0); i < size_.height*size_.width; ++i)
	{
		Magick::PixelPacket* pxl = pxls + i;
		Magick::Quantum color =
			static_cast<Magick::Quantum>
			(
				(
					(get_value_(index2pixel_(i)) - min_val) *
					static_cast<double>(std::numeric_limits<Magick::Quantum>::max() - 1)
				) / (max_val - min_val+1)
			);
		pxl->blue = color;
		pxl->red = color;
		pxl->green = color;
	}

	img.syncPixels();
	img.type(Magick::GrayscaleType);
	img.write(img_name);
}
