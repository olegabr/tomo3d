//transforms_ex.cpp
/*
* the transforms from object to shadow and vice versa
* .implementation.
* (C) 2005 olegabr. All rights reserved.
*/
#include <transforms.h>
#include "transforms_ex.h"
#include "shadow2d.h"
#include "filtered_shadow2d.h"
#include "object3d.h"
#include <vector>
#include <map>
#include <numeric>
#include <cmath>

#include <iostream>
using std::cout; using std::endl;

namespace
{
	size2d_t calc_pow2_size(size2d_t& sz)
	{
		size2d_t nsz;
		nsz.width = static_cast<size2d_t::value_type>(std::pow(2., std::floor(std::log((double)sz.width)/std::log(2.))/*+1*/));
		nsz.height = static_cast<size2d_t::value_type>(std::pow(2., std::floor(std::log((double)sz.height)/std::log(2.))/*+1*/));
		return nsz;
	}

	size2d_t bin_size(size2d_t& sz, int n)
	{
		size2d_t nsz(sz);
		//nsz.width  >> n;
		//nsz.height >> n;
		while(n-- > 0)
		{
			nsz.width /= 2;
			nsz.height /= 2;
		}
		return nsz;
	}

	projection2d::value_type get_val(projection2d& pr, const pixel2d& px)
	{
		projection2d::iterator i(pr.begin());
		std::advance(i, px.j);
		projection1d::iterator j(i->begin());
		std::advance(j, px.i);
		return *j;
	}
}
boost::shared_ptr<shadow2d> bin_by_pow2(shadow2d& sh, int n)
{
	size2d_t sz(sh.get_size()), sz2(calc_pow2_size(sz)), szb(bin_size(sz2, n));
	cout << "sz2 = " << sz2 << endl;
	cout << "szb = " << szb << endl;
	projection2d::iterator::difference_type
		di(sz2.height/szb.height), dj(sz2.width/szb.width);

	angle_t a(sh.begin()->get_angle());
	shadow2d sh2(angle_series_t(a, -a, sh.get_increment()), sz2);
	sh2.assign_and_rotate(sh, angle_t(0.));

	boost::shared_ptr<shadow2d> pshb(new shadow2d(angle_series_t(a, -a, sh2.get_increment()), szb));
	shadow2d::iterator p(sh2.begin()), e(sh2.end()), pb(pshb->begin());
	for (; p != e; ++p, ++pb)
	{
		projection2d::iterator bi(pb->begin()), be(pb->end());
		for (pixel2d px; bi != be; ++bi, px.j+=dj)
		{
			projection1d::iterator bii(bi->begin()), bie(bi->end());
			for (px.i = 0; bii != bie; ++bii, px.i+=di)
			{
				for (pixel2d d; d.i < di; ++d.i)
					for (d.j = 0; d.j < dj; ++d.j)
						*bii += get_val(*p, px+d);
				*bii /= (di*dj);
			}
		}
	}

	return pshb;
}


namespace std {
	typedef std::map<double, pixel2d> errors_type;
	std::ostream& operator << (std::ostream& os, const errors_type::value_type& val)
	{
		os << val.first << ", " << val.second;
		return os;
	}
}

void vary_alignment(shadow2d const& sh_, object3d const& object, int pxnum, shadow2d& sh1, shadow2d& sh2, std::vector<deviation_t>& vdev)
{
	typedef std::map<double, pixel2d> errors_type;

	double da(sh_.get_increment().to_radians());
	object3d object_(object);
	shadow2d sh(sh_);
	projection2d sav, p1;
	projection2d const& csav(sav);

	// the sh.norm is approximately constant while deviation is small
	shadow2d::value_type sh_norm2(0.);
	{
		filtered_shadow2d shf(sh, sh1);
		//shadow2d& shf(sh1);
		//shf = sh;
		//shf.ro_filtrate();
		sh_norm2 = sh*shf;
	}

	shadow2d::iterator p(sh.begin()), e(sh.end());
	for (; p != e; ++p)
	{
		errors_type errors;
		sav = *p;
		
		deviation_t dev;
		for (pixel2d d(-pxnum,-pxnum); d.i <= pxnum; ++d.i)
		{
			for (d.j = -pxnum; d.j <= pxnum; ++d.j)
			{
				dev.offset = point2d(d.j, d.i);
				p->assign_and_deviate(csav, dev);

				p1 = csav;
				p1.assign_difference(*p);
				p1.ro_filtrate();
				backproject(p1, object_, da);

				shadow2d& resh(sh1);
				resh.clear();
				project(object_, resh);
				//project_overwrite(object_, resh);
				shadow2d& diff(resh);
				diff.assign_difference(sh);

				filtered_shadow2d fdiff(diff, sh2);
				//shadow2d& fdiff(sh2);
				//fdiff = diff;
				//fdiff.ro_filtrate();
				shadow2d::value_type diff_norm2(diff*fdiff);

				errors[100*std::sqrt(diff_norm2/sh_norm2)] = d;

				object_ = object;
			}
		}
	std::copy
	(
		errors.begin(), errors.end(),
		std::ostream_iterator<errors_type::value_type>(cout, "\n")
	);
	cout << endl;

		cout << csav.get_angle() << "\t" << errors.begin()->second << '\t' << errors.begin()->first << '%' << endl << endl;
		pixel2d pd(errors.begin()->second);
		dev.offset = point2d(pd.j, pd.i);
		vdev.push_back(dev);

		p->assign_and_deviate(csav, dev);
		//p->swap(sav);
	}
}

void apply_deviations(shadow2d& sh, std::vector<deviation_t>& vdev, int bin_by2)
{
	assert(sh.get_proj_num() == vdev.size());

	point2d to_mass_center;
	std::vector<deviation_t>::iterator i(vdev.begin()), ie(vdev.end());
	for (; i != ie; ++i) to_mass_center += i->offset;
	for (i=vdev.begin(); i != ie; ++i) i->offset -= (1./vdev.size())*to_mass_center;

	shadow2d::iterator p(sh.begin()), e(sh.end());
	projection2d pr(p->get_size(), p->get_point_size(), p->get_angle());
	for (i=vdev.begin(); p != e && i != ie; ++p, ++i)
	{
		deviation_t d(*i);
		//d.offset *= std::pow(2., (bin_by2-1));
		pr.assign_and_deviate(*p, d);
		p->swap(pr);
	}
}
