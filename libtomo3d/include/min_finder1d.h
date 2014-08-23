//min_finder1d.h
/*
* the solver for minimum of 1d function
* (C) 2005 olegabr. All rights reserved.
*/
#ifndef LIBTOMO3D_MIN_FINDER1D_H
#define LIBTOMO3D_MIN_FINDER1D_H

#include <limits>
#include <utility>
#include <vector>
#include <algorithm>
using std::abs;

class min_finder1d_base
{
public :
	static double const ksi_;// = 0.38196601125; //2./(3+std::sqrt(5.));
};

template <class XType, class YType, class FType>
class min_finder1d :
	min_finder1d_base
{
public :
	typedef std::pair<XType, YType> x2y_pair;

private :
	typedef std::vector<x2y_pair> x2y_map;

	struct by_x
	{
		typedef x2y_pair arg_type;
		bool operator ()(const arg_type& lh, const arg_type& rh)
		{
			return lh.first <= rh.first;
		}
	};

	struct by_y
	{
		typedef x2y_pair arg_type;
		bool operator ()(const arg_type& lh, const arg_type& rh)
		{
			return lh.second <= rh.second;
		}
	};

	struct x_equals
	{
		typedef x2y_pair arg_type;
		x_equals(XType x, XType epsilon) :
			x_(x),
			epsilon_(epsilon) {}

		bool operator ()(const arg_type& a)
		{
			XType diff = a.first - x_;
			return abs(diff) < epsilon_;
		}

	private :
		const XType x_;
		const XType epsilon_;
	};

public :

	typedef XType xtype;
	typedef YType ytype;
	/* ytype f(xtype) */
	typedef FType functor_type;

	min_finder1d (xtype l, xtype r, int const I, xtype const epsilon)
		: l_(l), r_(r), I_(I), epsilon_(epsilon) {
		x2y_.reserve(4);
	}

	// returns a pair of x,y values of a found extremum
	x2y_pair operator ()(functor_type f) const
	{
		x2y_.clear();
		add_pair_(r_, f);
		add_pair_(l_, f);
		add_pair_(l_ + ksi_*(r_-l_), f);
		add_pair_(r_ - ksi_*(r_-l_), f);

		/* iteration loop */
		for (int i(0); i < I_; ++i)
		{
			sort_by_y_();
			ytype delta = y_(3) - y_(0);

			if (delta < 1.1 * std::numeric_limits<double>::epsilon()) break;
			erase_last_();

			sort_by_x_();
			if (x_(2) - x_(0) <= epsilon_) break;

         // error is accumulated here?
         // calculate x by multiplying end-points by gold number...
			xtype x = x_(0) + x_(2) - x_(1);
			if (has_xelement_(x)) break;
			add_pair_(x, f);
		}
		sort_by_y_();
		return x2y_[0];
	}

private :
	xtype x_(int i) const {
		return x2y_[i].first;
	}
	ytype y_(int i) const {
		return x2y_[i].second;
	}

	void erase_last_() const {
		x2y_.erase(x2y_.end()-1);
	}

	void add_pair_(xtype x, functor_type f) const {
		x2y_.push_back(std::make_pair(x, f(x)));
	}
	bool has_xelement_(xtype x) const
	{
		return std::find_if(x2y_.begin(), x2y_.end(), x_equals(x, epsilon_)) != x2y_.end();
	}
	void sort_by_y_() const
	{
		std::sort(x2y_.begin(), x2y_.end(), by_y());
	}
	void sort_by_x_() const
	{
		std::sort(x2y_.begin(), x2y_.end(), by_x());
	}

	const xtype l_, r_;
	const int I_; /* number of iterations */
	const xtype epsilon_;

	mutable x2y_map x2y_;
}; //min_finder1d

#endif //LIBTOMO3D_MIN_FINDER1D_H
