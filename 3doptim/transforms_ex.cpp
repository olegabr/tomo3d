//transforms_ex.cpp
/*
* the transforms from object to shadow and vice versa
* .implementation.
* (C) 2005 olegabr. All rights reserved.
*/
#include "transforms_ex.h"
#include "object3d.h"
#include "shadow2d.h"
#include <vector>
#include <algorithm>
#include <numeric>
#include <set>
#include <list>
#include <cmath>

#include <boost/lambda/lambda.hpp>

#include <iostream>
using std::cout; using std::endl;

shadow2d& create_mask(const shadow2d& sh, shadow2d& mask)
{
	shadow2d::const_iterator s = sh.begin(), send = sh.end();
	shadow2d::iterator m = mask.begin();
	for (; s != send; ++s, ++m)
	{
		size2d_t sz = m->get_size();
		projection2d::const_iterator ps = s->begin(), psend = s->end();
		int i1(0), i2(0), j1(0), j2(0);
		{
			projection1d::const_iterator pps = ps->begin(), ppsend = ps->end();
			for (; pps != ppsend; ++pps)
				if (*pps > 0.) break; else ++i1;
		}

		for (; ps != psend; ++ps)
			if (*(ps->end()-1) > 0.) break; else ++i2;

		for (ps = s->begin(); ps != psend; ++ps, ++j1)
			if (*(ps->begin()) > 0.) break;
		for (; ps != psend; ++ps, ++j1)
			if (*(ps->begin()) == 0.) break;

		{
			ps = psend-1;
			projection1d::const_iterator pps = ps->begin(), ppsend = ps->end();
			for (; pps != ppsend; ++pps, ++j2)
				if (*pps > 0.) break;
			for (; pps != ppsend; ++pps, ++j2)
				if (*pps == 0.) break;
		}

		{
			int offset = 2;
			projection2d::iterator pm = m->begin(), pmend = m->end();
			pixel2d px(0,0); //i,j
			for (; pm != pmend; ++pm, ++px.j)
			{
				projection1d::iterator ppm = pm->begin(), ppmend = pm->end();
				for (px.i = 0; ppm != ppmend; ++ppm, ++px.i)
				{
					if
					(
						(px.i == 0)||
						(px.i == (sz.height - 1))||
						((px.i+px.j) <= i1+offset)||
						((int(sz.height)+px.j-px.i-1) <= i2+offset)||
						((sz.width - 1 + px.i-px.j) <= sz.width - 1 - (j1-offset))||
						((sz.width - 1 + sz.height - 1 - px.i-px.j) <= sz.height - 1 - (j2-offset))
					) *ppm = 0.;
					else *ppm = 1.;
				}
			}
		}
	}

	return mask;
}

shadow2d& apply_mask(const shadow2d& mask, shadow2d& sh)
{
	//shadow2d::iterator s = sh.begin(), send = sh.end();
	//shadow2d::const_iterator m = mask.begin();
	//for (; s != send; ++s, ++m)
	//{
	//	projection2d::iterator ps = s->begin(), psend = s->end();
	//	projection2d::const_iterator pm = m->begin();
	//	for (; ps != psend; ++ps, ++pm)
	//	{
	//		projection1d::iterator pps = ps->begin(), ppsend = ps->end();
	//		projection1d::const_iterator ppm = pm->begin();
	//		for (; pps != ppsend; ++pps, ++ppm)
	//			//if (*ppm < 0.5) *pps = 0.;
	//			*pps *= *ppm;
	//	}
	//}
	return sh;
}

namespace {
	object3d::value_type calc_mean(const object3d& o, const pixel3d& px)
	{
		double k(1.1);
		object3d::value_type value(0);
		for (pixel3d d(-1,-1,-1); d.i <= 1; ++d.i)
			for (d.j = -1; d.j <= 1; ++d.j)
				for (d.k = -1; d.k <= 1; ++d.k)
					value += o(px+d);

		return k*( value )/27.;
	}

	bool is_hole(const object3d& o, const pixel3d& px)
	{
		const object3d::value_type z = 0.001;
		int n(0);
		for (pixel3d d(-1,-1,-1); d.i <= 1; ++d.i)
			for (d.j = -1; d.j <= 1; ++d.j)
				for (d.k = -1; d.k <= 1; ++d.k)
					n += ((o(px+d) > z) ? 1 : 0);
		return
		(
			o(px) < z && /* == 0.*/
			n >= (26 - 23)
		);
	}

	struct filled_area
	{
		typedef std::size_t hash_t;
		typedef std::set<hash_t> data_type;

		filled_area(const size3d_t& sz) : size_(sz)
		{
			//use size_.depth to store the multiplication for optimization purpose
			size_.depth = size_.width*size_.height;
		}
		filled_area& add_pixel(const pixel3d& px)
		{
			data_.insert(pixel2hash_(px));
			return *this;
		}
		filled_area& add_area(filled_area& ar)
		{
			assert(ar.size_ == size_);
			data_.insert(ar.data_.begin(), ar.data_.end());
			ar.data_.clear();
			return *this;
		}
		bool is_adjacent(const pixel3d& px)
		{
			for (pixel3d d(-1,-1,-1); d.i <= 1; ++d.i)
				for (d.j = -1; d.j <= 1; ++d.j)
					for (d.k = -1; d.k <= 1; ++d.k)
					{
						if (d == pixel3d()) continue;
						if ( find_pixel_(px+d) ) return true;
					}
			return false;
		}
		std::size_t size() const { return data_.size(); }
		void clear(object3d& o, object3d::value_type replace_val)
		{
			for (data_type::iterator i(data_.begin()); i != data_.end(); ++i)
				o(hash2pixel_(*i)) = replace_val;
			//data_.clear();
		}
	private :
		hash_t pixel2hash_(const pixel3d& px)
		{
			return (px.k*size_.height + px.i)*size_.width + px.j;//ind;
		}
		pixel3d hash2pixel_(hash_t ind)
		{
			pixel3d px;
			px.k = ind/size_.depth;
			px.i = (ind - px.k*size_.depth)/size_.width;
			px.j = ind - px.k*size_.depth - px.i*size_.width;
			return px;
		}
		bool find_pixel_(const pixel3d& px)
		{
			return data_.find(pixel2hash_(px)) != data_.end();
		}
		data_type data_;
		size3d_t size_;
	};

	bool operator < (const filled_area& lh, const filled_area& rh)
	{
		return lh.size() < rh.size();
	}

	void remove_small_areas(object3d& o, object3d::value_type search_val, object3d::value_type replace_val)
	{
		typedef std::list<filled_area> areas_type;
		typedef std::vector<areas_type::iterator> areas_ptrs_type;
		areas_type areas;
		size3d_t sz(o.get_size());
		long count(0), COUNT(norm(sz));
		for (pixel3d p(0, 0, 0); p.i < sz.height; ++p.i)
		{
			for (p.j = 0; p.j < sz.width; ++p.j)
			{
				for (p.k = 0; p.k < sz.depth; ++p.k)
				{ ++count;
					object3d::value_type v(o(p) - search_val);
					object3d::value_type absv(v>0 ? v : -v);
					if (absv < 0.0001)
					{
						areas_ptrs_type arp;
						for (areas_type::iterator a(areas.begin()); a != areas.end(); ++a)
							if ( a->is_adjacent(p) ) arp.push_back(a);

						if (arp.size() > 1)
						{
							//cout << "arp.size = " << arp.size() << "\n";
							//cout << count << '/' << COUNT << "\tareas.size = " << areas.size() << '\n';
							std::sort(arp.begin(), arp.end(), *boost::lambda::_1 < *boost::lambda::_2);
							areas_ptrs_type::iterator a(arp.begin()), big(arp.end()-1), aend(arp.end()-1);
							for (; a != aend; ++a)
							{
								(*big)->add_area(**a);
								areas.erase(*a);
							}

							(*big)->add_pixel(p);
						}
						else if (arp.empty())
						{
							//cout << "arp.empty" << "\n\n";
							filled_area a(sz);
							a.add_pixel(p);
							areas.push_back(a);
							cout << count << '/' << COUNT << "\tareas.size = " << areas.size() << '\n';
						}
						else /*arp.size() == 1*/
						{
							//cout << "add_pixel" << "\n\n";
							arp.front()->add_pixel(p);
						}
					}
				}
			}
		}
		cout << "areas.size() = " << areas.size() << endl;
		areas.sort();
		areas.reverse();
		areas.erase(areas.begin());
		for (areas_type::iterator a(areas.begin()); a != areas.end(); ++a)
			a->clear(o, replace_val);

	}
	void add_mass(object3d& o, object3d::value_type search_val, long V)
	{
		//filled_area area;
		//size3d_t sz(o.get_size());
		//long rV(0);
		//for (pixel3d p(0, 0, 0); p.i < sz.height; ++p.i)
		//	for (p.j = 0; p.j < sz.width; ++p.j)
		//		for (p.k = 0; p.k < sz.depth; ++p.k)
		//		{
		//			++rV;
		//			object3d::value_type v(o(p) - search_val);
		//			object3d::value_type absv(v>0 ? v : -v);
		//			if (absv < 0.0001) area.add_pixel(p);
		//		}
		//for(; rV < V; ++rV)
		//{
		//	area.add_pixel_to_border();
		//}
	}
} //unnamed namespace

/*
 * V = 427437 pixels
 * density = 1.85
 */
object3d& apply_filter(const object3d& obj, const point3d& center, object3d& result)
{
	assert(obj.get_size() == result.get_size());
	const pixel3d pc = obj.point2pixel(center);
	const long V = 216600;
	const double d = 1.85;
	double rd(d); // real density
	long rN(1); // number of pixels in count
	result(pc) = d;
	long rV(1); // number of pixels in new object
	const int rmax(38);
	for (int r(1); r <= rmax; ++r)
	{
		/*
		 * calc main volume
		 */
		{pixel3d dp(-r, -r, -r);
			for (; dp.j <= r; ++dp.j)
			{
				for (dp.k = -r; dp.k <= r; ++dp.k)
				{
					pixel3d px(pc);
					px += dp;
					rd += obj(px);
					++rN;
					if (calc_mean(obj, px) >= (rd/rN))
					{
						++rV;
						result(px) = d;
					}
				}
			}
		}
		{pixel3d dp(r, -r, -r);
			for (; dp.j <= r; ++dp.j)
			{
				for (dp.k = -r; dp.k <= r; ++dp.k)
				{
					pixel3d px(pc);
					px += dp;
					rd += obj(px);
					++rN;
					if (calc_mean(obj, px) >= (rd/rN))
					{
						++rV;
						result(px) = d;
					}
				}
			}
		}
		{pixel3d dp(1-r, -r, -r);
			for (; dp.i < r; ++dp.i)
			{
				for (dp.k = -r; dp.k <= r; ++dp.k)
				{
					pixel3d px(pc);
					px += dp;
					rd += obj(px);
					++rN;
					if (calc_mean(obj, px) >= (rd/rN))
					{
						++rV;
						result(px) = d;
					}
				}
			}
		}
		{pixel3d dp(1-r, r, -r);
			for (; dp.i < r; ++dp.i)
			{
				for (dp.k = -r; dp.k <= r; ++dp.k)
				{
					pixel3d px(pc);
					px += dp;
					rd += obj(px);
					++rN;
					if (calc_mean(obj, px) >= (rd/rN))
					{
						++rV;
						result(px) = d;
					}
				}
			}
		}
		{pixel3d dp(1-r, 1-r, -r);
			for (; dp.j < r; ++dp.j)
			{
				for (dp.i = 1-r; dp.i < r; ++dp.i)
				{
					pixel3d px(pc);
					px += dp;
					rd += obj(px);
					++rN;
					if (calc_mean(obj, px) >= (rd/rN))
					{
						++rV;
						result(px) = d;
					}
				}
			}
		}
		{pixel3d dp(1-r, 1-r, r);
			for (; dp.j < r; ++dp.j)
			{
				for (dp.i = 1-r; dp.i < r; ++dp.i)
				{
					pixel3d px(pc);
					px += dp;
					rd += obj(px);
					++rN;
					if (calc_mean(obj, px) >= (rd/rN))
					{
						++rV;
						result(px) = d;
					}
				}
			}
		}

		/*
		 * fill holes
		 */
		--r;
		{pixel3d dp(-r, -r, -r);
			for (; dp.j <= r; ++dp.j)
			{
				for (dp.k = 0; dp.k <= r; ++dp.k)
				{
					pixel3d px(pc);
					px += dp;
					if (is_hole(obj, px))
					{
						++rV;
						result(px) = d;
					}
				}
			}
		}
		{pixel3d dp(r, -r, -r);
			for (; dp.j <= r; ++dp.j)
			{
				for (dp.k = 0; dp.k <= r; ++dp.k)
				{
					pixel3d px(pc);
					px += dp;
					if (is_hole(obj, px))
					{
						++rV;
						result(px) = d;
					}
				}
			}
		}
		{pixel3d dp(1-r, -r, -r);
			for (; dp.i < r; ++dp.i)
			{
				for (dp.k = 0; dp.k <= r; ++dp.k)
				{
					pixel3d px(pc);
					px += dp;
					if (is_hole(obj, px))
					{
						++rV;
						result(px) = d;
					}
				}
			}
		}
		{pixel3d dp(1-r, r, -r);
			for (; dp.i < r; ++dp.i)
			{
				for (dp.k = 0; dp.k <= r; ++dp.k)
				{
					pixel3d px(pc);
					px += dp;
					if (is_hole(obj, px))
					{
						++rV;
						result(px) = d;
					}
				}
			}
		}
		{pixel3d dp(1-r, 1-r, -r);
			for (; dp.j < r; ++dp.j)
			{
				for (dp.i = 0; dp.i < r; ++dp.i)
				{
					pixel3d px(pc);
					px += dp;
					if (is_hole(obj, px))
					{
						++rV;
						result(px) = d;
					}
				}
			}
		}
		{pixel3d dp(1-r, 1-r, r);
			for (; dp.j < r; ++dp.j)
			{
				for (dp.i = 0; dp.i < r; ++dp.i)
				{
					pixel3d px(pc);
					px += dp;
					if (is_hole(obj, px))
					{
						++rV;
						result(px) = d;
					}
				}
			}
		}
		++r;
		/*
		 * end of fill holes
		 */
		if (rV >= V) break;
	}

	remove_small_areas(result, d, 0.);
	remove_small_areas(result, 0., d);
	//add_mass(result, d, V);
	//cout << "new Volume = " << rV << endl;

	return result;
}


point3d calc_center(const shadow2d& sh)
{
	point3d p;
	shadow2d::const_iterator pr = sh.begin(), epr = sh.end();
	for (; pr != epr; ++pr)
		if (abs(pr->get_angle()) <= angle_t(43)) break;
	point2d p1 = pr->get_mass_center();
	angle_t a1 = pr->get_angle();
	for (; pr != epr; ++pr)
		if (pr->get_angle() == angle_t(0)) break;
	point2d p2 = pr->get_mass_center();
	angle_t a2 = pr->get_angle();

	p.z = .5*(p1.x + p2.x);
	p.y = (p1.y*cos(a2) - p2.y*cos(a1))/(sin(a1)*cos(a2) - sin(a2)*cos(a1));
	p.x = (p1.y - p.y*sin(a1))/cos(a1);

	return p;
}

object3d& filter_object(object3d& object)
{
	const int mean = 167;
	object3d::iterator o2d(object.begin()), end1(object.end());
	for (; o2d != end1; ++o2d)
	{
		object2d::iterator o(o2d->begin()), end2(o2d->end());
		for (; o != end2; ++o)
		{
			double d(*o-mean);
			*o = std::pow((d>0?d:0.), 10.);
		}
	}
	return object;
}

void black_threshold(shadow2d& sh, double threshold)
{
	shadow2d::iterator s = sh.begin(), send = sh.end();
	for (; s != send; ++s)
	{
		projection2d::iterator ps = s->begin(), psend = s->end();
		for (; ps != psend; ++ps)
		{
			projection1d::iterator pps = ps->begin(), ppsend = ps->end();
			for (; pps != ppsend; ++pps)
			{
				projection1d::value_type val = *pps;
				*pps = (val >= threshold) ? val : 0.;
			}
		}
	}
}
