//fft2d.cpp
/*
* the utility for 2d fft
* (C) 2005 olegabr. All rights reserved.
*/

#include <fftw++.h>
#include <algorithm>
#include <cassert>
#include "img_generator.h"

namespace detail {
class fft_helper_t
{
public :
	typedef double value_type;
	fft_helper_t();
	~fft_helper_t();
	void set_image(img_generator& ig);
	void fft();
	value_type norm2(double qmax);
private :
	std::size_t get_sz2_(std::size_t sz);
	void init_(img_generator& ig);
	void release_();
	void fill_(img_generator& ig);

	std::size_t szx_, szy_, szf_, szg_;
	value_type *f_;
	Complex*    g_;
	rcfft2d* pForward_;
};
} //namespace detail

detail::fft_helper_t::fft_helper_t() : szx_(0)
{
	//init_(sz);
}
detail::fft_helper_t::~fft_helper_t()
{
	release_();
}

detail::fft_helper_t::value_type detail::fft_helper_t::norm2(double qmax)
{
	detail::fft_helper_t::value_type result(0);
	std::size_t const W(szx_);
	double const qmax2(qmax*qmax);
	for (std::size_t i(0); i < szg_; ++i)
	{
		std::size_t iy(i/W), ix(i-iy*W);
		detail::fft_helper_t::value_type
			n(std::pow((ix-W/2)/(1.*W), 2.) + std::pow((1.*iy*iy)/szy_, 2.));
		if (n > qmax2) result += norm(g_[i]);
	}
	return result;
}

void detail::fft_helper_t::set_image(img_generator& ig)
{
	if (szx_ == 0) init_(ig);
	else fill_(ig);
}

void detail::fft_helper_t::fft()
{
	pForward_->fft0(f_, g_);
}

void detail::fft_helper_t::fill_(img_generator& ig)
{
	std::size_t i(0);
	for (; i < szx_*szy_; ++i, ++ig) f_[i] = *ig;
	for (; i < szf_; ++i) f_[i] = 0;
}

std::size_t detail::fft_helper_t::get_sz2_(std::size_t sz)
{
	return// (2*sz-1) << 1;
		1 << 
			static_cast<std::size_t>
			(
				std::ceil
				(
					std::log(2.*sz-1.)/std::log(2.)
				)
			);
}

void detail::fft_helper_t::init_(img_generator& ig)
{
	szx_ = ig.get_width();
	szy_ = ig.get_height();
	szf_ = get_sz2_(szx_*szy_);
	f_ = FFTWdouble(szf_);
	fill_(ig);

	szg_ = szx_*(szy_/2+1);
	g_ = FFTWComplex(szg_);
	pForward_ = new rcfft2d(szx_, szy_, f_, g_);
}

void detail::fft_helper_t::release_()
{
	delete pForward_;
	FFTWdelete(g_);
	FFTWdelete(f_);
}

double ftnorm2(img_generator& ig, double qmax/*0..0,5*/)
{
	static detail::fft_helper_t fft_helper;
	fft_helper.set_image(ig);
	fft_helper.fft();

	return fft_helper.norm2(qmax);
}
