//ro_filtrate.cpp
/*
* the utility for ro-filtration
* (C) 2005 olegabr. All rights reserved.
*/

#include <fftw++.h>
#include <algorithm>
#include "ro_filtrate.h"
#include "angle.h"
#include "detail/fft_helper.h"

#include <cassert>

namespace {

double window(double n, double N)
{
	assert(n<=N);
	double y0 = 0.;//0.08;
	double c = std::cos(angle_t::PI*n/N);
	return .5*((1+y0) + (1-y0)*c);
	//return 0.54 - 0.46*std::cos(2*angle_t::PI*n/N);
}

} // unnamed namespace

detail::fft_helper_t::fft_helper_t(std::size_t sz)
{
	init_(sz);
}
detail::fft_helper_t::~fft_helper_t()
{
	release_();
}
void detail::fft_helper_t::set_size(std::size_t sz)
{
	//assert(sz == sz_);
	if (sz <= sz_)
	{
		sz_ = sz;
		std::fill(f_+sz_, f_+szf_, 0.);
	}
	else if (szf_ < get_sz2_(sz))
	{
		std::cout << "fft_helper_t::set_size: sz > sz_\n";
		release_();
		init_(sz);
	}

	//if (sz_ < sz && szf_ < get_sz2(sz))
	//{
	//	std::cout << "fft_helper_t::set_size: sz > sz_\n";
	//	release();
	//	init(sz);
	//}
	//else
	//{
	//	sz_ = sz;
	//	std::fill(f_+sz_, f_+szf_, 0.);
	//}
}

detail::fft_helper_t::value_type* detail::fft_helper_t::get_data()
{
	std::fill(f_+sz_, f_+szf_, 0.);
	return f_;
}

Complex* detail::fft_helper_t::get_g_()
{
	return g_;
}

void detail::fft_helper_t::fft()
{
	pForward_->fft(f_, g_);
}
void detail::fft_helper_t::mul_by_ro()
{
	detail::fft_helper_t::value_type ro(0);
	for (std::size_t i(0); i < szg_; ++i, ro += ro_) g_[i] *= ro;
}
void detail::fft_helper_t::apply_window()
{
	detail::fft_helper_t::value_type
		norm1 = .5*(std::norm(g_[0]) + std::norm(g_[szg_-1]));
	for (std::size_t i(1); i < szg_-1; ++i) norm1 += std::norm(g_[i]);

	for (std::size_t i(0); i < szg_; ++i) g_[i] *= window(i, szg_);

	detail::fft_helper_t::value_type
		norm2 = .5*(std::norm(g_[0]) + std::norm(g_[szg_-1]));
	for (std::size_t i(1); i < szg_-1; ++i) norm2 += std::norm(g_[i]);

	detail::fft_helper_t::value_type
		k = std::sqrt(norm1/(norm2==0.?1:norm2));
	for (std::size_t i(0); i < szg_; ++i) g_[i] *= k;
}
void detail::fft_helper_t::ifft()
{
	pBackward_->fftNormalized(g_, f_);
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

void detail::fft_helper_t::init_(std::size_t sz)
{
	sz_  = sz;
	szf_ = get_sz2_(sz);
	f_ = FFTWdouble(szf_);
	std::fill(f_+sz_, f_+szf_, 0.);

	szg_ = szf_/2+1;
	ro_  = 0.5/szg_;
	g_ = FFTWComplex(szg_);
	pForward_  = new rcfft1d(szf_, f_, g_);
	pBackward_ = new crfft1d(szf_, g_, f_);
}

void detail::fft_helper_t::release_()
{
	delete pBackward_;
	delete pForward_;
	FFTWdelete(g_);
	FFTWdelete(f_);
}

void util::ro_filtrate(std::vector<double>& v)
{
	static detail::fft_helper_t fft_helper(v.size());
	fft_helper.set_size(v.size());

	double *f = fft_helper.get_data();
	std::copy(v.begin(), v.end(), f);

	fft_helper.fft();
	fft_helper.mul_by_ro();
	fft_helper.apply_window();
	fft_helper.ifft();

	std::copy(f,f+v.size(),v.begin());
}
