//fft_helper.h
/*
* helper class for ro-filtration utilty
* (C) 2005 olegabr. All rights reserved.
*/
#ifndef LIBTOMO3D_DETAIL_FFT_HELPER_H
#define LIBTOMO3D_DETAIL_FFT_HELPER_H

#include <complex>
typedef std::complex<double> Complex;

//#include <fftw++.h>
class rcfft1d;
class crfft1d;

namespace detail {
class fft_helper_t
{
public :
	typedef double value_type;
	fft_helper_t(std::size_t sz);
	~fft_helper_t();
	void set_size(std::size_t sz);
	value_type* get_data();
	void fft();
	void mul_by_ro();
	void apply_window();
    void ifft();

	Complex* get_g_();
private :
	std::size_t get_sz2_(std::size_t sz);
	void init_(std::size_t sz);
	void release_();

	std::size_t sz_, szf_, szg_;
	value_type ro_;
	value_type *f_;
	Complex*    g_;
	rcfft1d* pForward_;
	crfft1d* pBackward_;
};
} //namespace detail

#endif //LIBTOMO3D_DETAIL_FFT_HELPER_H
