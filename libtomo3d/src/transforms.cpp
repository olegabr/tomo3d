//transforms.cpp
/*
* the transforms from object to shadow and vice versa
* .implementation.
* (C) 2005 olegabr. All rights reserved.
*/
#include "transforms.h"
#include "object3d.h"
#include "shadow2d.h"
#include "filtered_shadow2d.h"
#include <numeric>
#include <algorithm>

namespace {
	projection1d& project(const object2d& o2d, projection1d& prj, const angle_t& a)
	{
		point2d::value_type d = o2d.get_point_size();
		point1d dR = 0.5*d*(cos(a)+sin(abs(a)));
		angle_t a1 = abs(a);
		a1 = (a1 > angle_t(45)) ? (angle_t(90) - a1) : a1;

		/* optimization! */
		point1d
			R1_(0.5*(cos(a1) - sin(a1))),
			R2_(0.5*(sin(a1)+cos(a1))),
			L1_(1./cos(a1)),
			L2_(1./(sin(a1)*cos(a1)));

		//iterate along the 2D slice
		for (object2d::const_iterator o = o2d.begin(); o != o2d.end(); ++o)
		{
			point1d
				R0   = o.get_point()*a,
				Rmin = R0 - dR, Rmax = R0 + dR,
				pointsz(prj.get_point_size());
			for (point1d R(prj.get_point_gt(Rmin)); R <= Rmax; R += pointsz)
			{
				point1d R1 = /*std::abs*/((R < R0)?(R0 - R):(R - R0))/*/d FIXME!!! if d != 1*/;
				point1d L = (R1 <= R1_) ? L1_ : (R2_ - R1)*L2_;
				prj.add_value(R, (((R==Rmin)||(R==Rmax)) ? 0.5 : 1) * L * *o);
			}
		}

		return prj;
	}

	projection1d& project_overwrite(const object2d& o2d, projection1d& prj, const angle_t& a)
	{
		prj.clear();
		return project(o2d, prj, a);
	}
} //unnamed namespace

shadow2d& project(const object3d& object, shadow2d& shdw)
{
	//iterate through 2D slices
	object3d::const_iterator o2d(object.begin()), o2dend(object.end());
	for (; o2d != o2dend; ++o2d)
	{
		//iterate through 2D projections
		pixel1d z(o2d.get_position_index());
		const object2d& ro2d(*o2d);
		shadow2d::iterator prj2d(shdw.begin()), prj2dend(shdw.end());
		for (; prj2d != prj2dend; ++prj2d)
			project(ro2d, prj2d->get_slice(z), prj2d->get_angle());
	}
	return shdw;
}

shadow2d& project_overwrite(const object3d& object, shadow2d& shdw)
{
	shdw.clear();
	return project(object, shdw);
}

object3d& backproject(const filtered_shadow2d& fsh, object3d& object, double betha)
{
	object3d::value_type d = betha*fsh.get_increment().to_radians();
	object3d::iterator o2d = object.begin();
	object3d::iterator end1 = object.end();
	shadow2d::const_iterator pend = fsh.end();
	for (; o2d != end1; ++o2d)
	{
		point2d::value_type x = o2d.get_position();
		object2d::iterator o = o2d->begin(), end2 = o2d->end();
		for (; o != end2; ++o)
		{
			point2d p = o.get_point();
			shadow2d::const_iterator prj2d = fsh.begin();
			projection2d::value_type val(0);
			for (; prj2d != pend; ++prj2d)
				val += prj2d->get_value(point2d(x, p*prj2d->get_angle()));
			*o += d*val;
			if (*o < 0.0) *o = 0.0; // eliminate_negativeness
		}
	}
	return object;
}

object3d& backproject_overwrite(const filtered_shadow2d& fsh, object3d& object, double betha)
{
	object3d::value_type d = betha*fsh.get_increment().to_radians();
	object3d::iterator o2d = object.begin();
	object3d::iterator end1 = object.end();
	shadow2d::const_iterator pend = fsh.end();
	for (; o2d != end1; ++o2d)
	{
		point2d::value_type x = o2d.get_position();
		object2d::iterator o = o2d->begin(), end2 = o2d->end();
		for (; o != end2; ++o)
		{
			point2d p = o.get_point();
			shadow2d::const_iterator prj2d = fsh.begin();
			projection2d::value_type val(0);
			for (; prj2d != pend; ++prj2d)
				val += prj2d->get_value(point2d(x, p*prj2d->get_angle()));
			*o = d*val;
			if (*o < 0.0) *o = 0.0; // eliminate_negativeness
		}
	}
	return object;
}

object3d& backproject(const projection2d& fp, object3d& object, double betha)
{
	object3d::value_type d = betha;
	object3d::iterator o2d = object.begin();
	object3d::iterator end1 = object.end();
	for (; o2d != end1; ++o2d)
	{
		point2d::value_type x = o2d.get_position();
		object2d::iterator o = o2d->begin(), end2 = o2d->end();
		for (; o != end2; ++o)
		{
			point2d p = o.get_point();
			projection2d::value_type val(0);
			val += fp.get_value(point2d(x, p*fp.get_angle()));
			*o += d*val;
			if (*o < 0.0) *o = 0.0; // eliminate_negativeness
		}
	}
	return object;
}

object3d& backproject_overwrite(const projection2d& fp, object3d& object, double betha)
{
	object3d::value_type d = betha;
	object3d::iterator o2d = object.begin();
	object3d::iterator end1 = object.end();
	for (; o2d != end1; ++o2d)
	{
		point2d::value_type x = o2d.get_position();
		object2d::iterator o = o2d->begin(), end2 = o2d->end();
		for (; o != end2; ++o)
		{
			point2d p = o.get_point();
			projection2d::value_type val(0);
			val += fp.get_value(point2d(x, p*fp.get_angle()));
			*o = d*val;
			if (*o < 0.0) *o = 0.0; // eliminate_negativeness
		}
	}
	return object;
}

namespace {
projection1d::value_type cut_negative(projection1d::value_type v)
{
	return v>0.? v : 0.;
}
}

shadow2d& normalize(shadow2d& sh)
{
	std::vector<double> v;
	shadow2d::iterator s = sh.begin(), send = sh.end();
	for (; s != send; ++s)
	{
		projection1d::value_type min_val(66000);
		projection2d::iterator ps = s->begin(), psend = s->end();
		for (; ps != psend; ++ps)
		{
			projection1d::value_type val(*std::min_element(ps->begin(), ps->end()));
			min_val = min_val<val ? min_val : val;
		}
		min_val *= 1;//.2; // + 20%

		for (ps = s->begin(); ps != psend; ++ps)
		{
			projection1d::iterator pps = ps->begin(), ppsend = ps->end();
			for (; pps != ppsend; ++pps)
				*pps = cut_negative(*pps - min_val);
		}

		double val(0);
		for (ps = s->begin(); ps != psend; ++ps)
		{
			projection1d::iterator pps = ps->begin(), ppsend = ps->end();
			for (; pps != ppsend; ++pps) val += *pps;
		}
		v.push_back(val);
	}
	double sum = std::accumulate(v.begin(), v.end(), 0.)/v.size();

	std::vector<double>::iterator iv = v.begin();
	for (s = sh.begin(); s != send; ++s, ++iv)
	{
		projection2d::iterator ps = s->begin(), psend = s->end();
		for (; ps != psend; ++ps)
		{
			projection1d::iterator pps = ps->begin(), ppsend = ps->end();
			for (; pps != ppsend; ++pps) *pps *= (sum/(*iv));
		}
	}

	return sh;
}

shadow2d& align_to_gravity_center(shadow2d& sh)
{
	point2d center;
	//typedef std::vector<point2d> centers_type;
	//centers_type centers(sh.get_proj_num());
	//centers_type::iterator c(centers.begin());
	shadow2d::iterator p(sh.begin()), e(sh.end());
	for (; p != e; ++p/*, ++c*/)
		//*c = p->get_mass_center();
		center += p->get_mass_center();
	center *= (1./sh.get_proj_num());

	p = sh.begin();
	//c = centers.begin();
	projection2d pr(p->get_size(), p->get_point_size(), p->get_angle());
	for (; p != e; ++p/*, ++c*/)
	{
		deviation_t d(/**c*/center);
		pr.assign_and_deviate(*p, d);
		pr.swap(*p);
	}

	return sh;
}
