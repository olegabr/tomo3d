//shadow2d.h
/*
* the shadow2d class
* (C) 2005 olegabr. All rights reserved.
*/
#ifndef LIBTOMO3D_SHADOW2D_H
#define LIBTOMO3D_SHADOW2D_H

#include "angle.h"
#include "projection2d.h"
#include <vector>
#include <map>
#include <iosfwd>

struct angle_series_t
{
	angle_t start, end, increment;
	angle_series_t(const angle_t& start_, const angle_t& end_, const angle_t& increment_)
		: start(start_), end(end_), increment(increment_) {}
};

/* returns the number of projections in this series */
std::size_t norm(const angle_series_t& as);

/*
 тень это набор проекций обьекта под разными углами.
*/
class shadow2d
{
public :
	typedef projection2d::value_type value_type;
	typedef std::vector<projection2d> data_type;

	//shadow2d(); //debug
	shadow2d(const angle_series_t& series, size2d_t projection_size = size2d_t());
	void load_from_image(const std::string& dir);
	~shadow2d();
	shadow2d(const shadow2d& other);
	shadow2d& operator =(const shadow2d& other);
	// g = f - g;
	shadow2d& assign_difference(const shadow2d& f);
	shadow2d& assign_and_rotate(const shadow2d&, const angle_t&);
	shadow2d& assign_and_deviate(const shadow2d&, const deviation_t&);
	void clear();
	friend void swap(shadow2d&, shadow2d&);
	void save_to_image(std::string img_name) const;

	//in place filtration
	void ro_filtrate();
	//// the dot product of two shadows
	//friend value_type
	//	operator *(const shadow2d& sh, const shadow2d& fsh);
	//value_type norm2() const;

	angle_t get_increment() const { return increment_; }
	angle_series_t get_angle_series() const {
		return m_angle_series;
	}
	size2d_t get_size() const;
	std::size_t get_proj_num() const { return data_.size(); }
	point2d::value_type get_point_size() const;
	//debug
	friend std::ostream&
		operator <<(std::ostream& os, const shadow2d& sh);

	//class iterator
	//{
	//public:
	//	iterator() {}
	//	iterator(shadow2d::data_type::iterator iprojection)
	//		: iprojection_(iprojection) {}
	//	iterator(const iterator& i)
	//		: iprojection_(i.iprojection_) {}
	//	projection2d* operator ->();
	//	projection2d& operator * ();
	//	iterator&  operator ++();
	//	friend bool
	//		operator != (const iterator& lh, const iterator& rh);

	//private :
	//	shadow2d::data_type::iterator iprojection_;
	//};
	typedef data_type::iterator iterator;

	//iterator begin() { return iterator(data_.begin()); }
	//iterator end  () { return iterator(data_.end()); }
	iterator begin() {
		return data_.begin();
	}
	iterator end () {
		return data_.end();
	}

	//class const_iterator
	//{
	//public:
	//	const_iterator() {}
	//	const_iterator(shadow2d::data_type::const_iterator iprojection)
	//		: iprojection_(iprojection) {}
	//	const_iterator(const const_iterator& i)
	//		: iprojection_(i.iprojection_) {}
	//	const projection2d* operator ->() const;
	//	const projection2d& operator * () const;
	//	const_iterator&  operator ++();
	//	friend bool
	//		operator != (const const_iterator& lh, const const_iterator& rh);

	//private :
	//	shadow2d::data_type::const_iterator iprojection_;
	//};
	typedef data_type::const_iterator const_iterator;

	//const_iterator begin() const { return const_iterator(data_.begin()); }
	//const_iterator end  () const { return const_iterator(data_.end()); }
	const_iterator begin() const {
		return data_.begin();
	}
	const_iterator end () const {
		return data_.end();
	}

private :
	//void clear();

	std::pair<value_type, value_type> get_min_max_() const;

	data_type data_;
	const angle_t increment_;
	angle_series_t m_angle_series;
};

//inline projection2d* shadow2d::iterator::operator ->()
//{
//	return &*iprojection_;
//}
//
//inline projection2d& shadow2d::iterator::operator *()
//{
//	return *iprojection_;
//}
//
//inline shadow2d::iterator& shadow2d::iterator::operator ++()
//{
//	++iprojection_;
//	return *this;
//}
//
//inline const projection2d* shadow2d::const_iterator::operator ->() const
//{
//	return &*iprojection_;
//}
//
//inline const projection2d& shadow2d::const_iterator::operator  *() const
//{
//	return *iprojection_;
//}
//inline shadow2d::const_iterator& shadow2d::const_iterator::operator ++()
//{
//	++iprojection_;
//	return *this;
//}

#endif //LIBTOMO3D_SHADOW2D_H
