//img_generator.h
/*
* helper class for ro-filtration utilty
* (C) 2005 olegabr. All rights reserved.
*/
#ifndef IMG_GENERATOR_H
#define IMG_GENERATOR_H

#include <Magick++.h>

typedef double koefficient_type;
typedef double image_norm_type;

typedef Magick::PixelPacket* PPIterator;
typedef Magick::PixelPacket const* PPConstIterator;

inline double
calc_img_value(Magick::Quantum i, Magick::Quantum z, koefficient_type k, Magick::Quantum o, double g_max, Magick::Quantum g)
{
	return (i - z + k*o*g_max/g);
}

class img_generator
{
public :
	typedef double value_type;
	img_generator
	(
		PPConstIterator opxl,
		PPConstIterator gpxl,
		PPConstIterator zpxl,
		PPIterator ipxl,
		std::size_t width,
		std::size_t height,
		double gain_max,
		koefficient_type k
	) : o_(opxl), g_(gpxl), z_(zpxl), i_(ipxl), iend_(ipxl+width*height),
		width_(width), height_(height), gain_max_(gain_max), k_(k) {}

	bool operator ++()
	{
		if (i_ == iend_) return false;
		++i_; ++z_; ++o_; ++g_;
		return true;
	}
	value_type operator *() const
	{
		return calc_img_value(i_->red, z_->red, k_ , o_->red, gain_max_, g_->red);
	}
	std::size_t get_height() const
	{
		return height_;
	}
	std::size_t get_width() const
	{
		return width_;
	}
private :
	PPConstIterator o_, g_, z_;
	const PPIterator iend_;
	std::size_t width_, height_;
	PPIterator i_;
	const double gain_max_;
	const koefficient_type k_;
};

#endif //IMG_GENERATOR_H
