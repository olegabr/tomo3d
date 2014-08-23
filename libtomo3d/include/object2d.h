//object2d.h
/*
* the 2d object
* (C) 2004 olegabr. All rights reserved.
*/
#ifndef LIBTOMO3D_OBJECT2D_H
#define LIBTOMO3D_OBJECT2D_H

#include <cassert>
#include <vector>
#include <string>
#include <map>
#include "point2d.h"
#include "size2d.h"
#include "pixel2d.h"

// это исходный объект. в данном случае 2D.
// система координат в центре объекта (H/2;W/2)
// ось x горизонтально слева направо
// ось y вертикально снизу вверх.
class object2d
{
public :
	typedef double value_type;
	typedef std::vector<value_type> data_type;

	object2d();//for debug
	object2d(const size2d_t& sz, point2d::value_type point_size = 1);
	object2d(const object2d&);

	//construct from image
	object2d(std::string image_name);
	void save_to_image(std::string image_name, std::pair<value_type, value_type> const &min_max) const;
	void load_from_image(const std::string& image_name);
	friend std::ostream& operator <<(std::ostream& os, const object2d& obj);

	value_type norm2() const;
	//void eliminate_negativeness();
	void clear();

	void add_scaled_diff(object2d const& diff, double betha = 1.0);
	void assign_scaled_diff(object2d const& diff, double betha = 1.0);

	point2d::value_type get_point_size() const { return point_size_; }
	size2d_t get_size() const { return size_; }

	class iterator
	{
		enum {END = -1};
	public:
		iterator()
			: object_(0), index_(END) {}
		iterator(object2d* object)
			: object_(object), index_(0) { operator ++(); }
		iterator(const iterator& i)
			: object_(i.object_), index_(i.index_) {}
		object2d::value_type& operator *();
		iterator& operator ++();
		point2d get_point() const;
		friend bool
			operator != (const iterator& lh, const iterator& rh);

	private :
		int index_;
		object2d* object_;
	};
	friend class iterator;

	iterator begin() { return iterator(this); }
	iterator end  () { return iterator(); }

	class const_iterator
	{
		enum {END = -1};
	public:
		const_iterator()
			: object_(0), index_(END) {}
		const_iterator(const object2d* object)
			: object_(object), index_(0) { operator ++(); }
		const_iterator(const const_iterator& i)
			: object_(i.object_), index_(i.index_) {}
		const_iterator& operator ++();
		object2d::value_type operator *() const;
		point2d get_point() const;
		//value_type calc_integral(const point2d& p1, const point2d& p2) const;
		friend bool
			operator != (const const_iterator& lh, const const_iterator& rh);

	private :
		int index_;
		const object2d* object_;
	};
	friend class const_iterator;

	const_iterator begin() const { return const_iterator(this); }
	const_iterator end  () const { return const_iterator(); }

	value_type& operator ()(const pixel2d&);
	value_type operator ()(const pixel2d&) const;

	pixel2d point2pixel(const point2d&) const;

	std::pair<value_type, value_type> get_min_max_() const;

private :
	point2d::value_type get_width_() const { return (size_.width - 1)*point_size_; }
	point2d::value_type get_height_() const { return (size_.height - 1)*point_size_; }

	//point2d::value_type get_xmax_() const { return 0.5*get_width_(); }
	point2d::value_type get_xmin_() const { return -0.5*get_width_(); }
	point2d::value_type get_ymax_() const { return 0.5*get_height_(); }

	point2d pixel2point_(const pixel2d& px) const;
	pixel2d index2pixel_(std::size_t i) const;
	std::size_t pixel2index_(const pixel2d& px) const;

	//variables:
	data_type data_;
	size2d_t size_; //число точек по каждой из координат
	point2d::value_type point_size_; //размеры точки по высоте и ширине
};

inline object2d::value_type& object2d::iterator::operator *()
{
	assert(in_grid(index_, object_->data_.size()));
	return object_->data_[index_];
}

inline object2d::iterator& object2d::iterator::operator ++()
{
	assert(in_grid(index_, object_->data_.size()));

	int N(static_cast<int>(object_->data_.size())-1);
	size2d_t::value_type h(object_->get_size().height);
	double h2(/*0.9*0.9**/0.25*h*h);
	point2d p;
	do
	{
		if (index_ == N)
		{
			index_ = object2d::iterator::END;
			break;
		}
		else ++index_;
		p = object_->pixel2point_(object_->index2pixel_(index_));
	}
	while (p.x*p.x + p.y*p.y >= h2);
	return *this;
}

inline point2d object2d::iterator::get_point() const
{
	assert(in_grid(index_, object_->data_.size()));
	return object_->pixel2point_(object_->index2pixel_(index_));
}

inline object2d::const_iterator& object2d::const_iterator::operator ++()
{
	assert(in_grid(index_, object_->data_.size()));

	int N(static_cast<int>(object_->data_.size()-1));
	size2d_t::value_type h(object_->get_size().height);
	double h2(0.25*h*h);
	point2d p;
	do
	{
		if (index_ == N)
		{
			index_ = object2d::const_iterator::END;
			break;
		}
		else ++index_;
		p = object_->pixel2point_(object_->index2pixel_(index_));
	}
	while (p.x*p.x + p.y*p.y >= h2);
	return *this;
}

inline object2d::value_type object2d::const_iterator::operator *() const
{
	assert(in_grid(index_, object_->data_.size()));
	return object_->data_[index_];
}

inline point2d object2d::const_iterator::get_point() const
{
	assert(in_grid(index_, object_->data_.size()));
	return object_->pixel2point_(object_->index2pixel_(index_));
}

//inline object2d::value_type
//object2d::const_iterator::calc_integral(const point2d& p1, const point2d& p2) const
//{
//	return norm(p1-p2)*object_->data_[index_];
//	//pixel2d px = object_->index2pixel(index_);
//	//std::size_t
//	//	i1 = index_,
//	//	i2 = object_->pixel2index(px.right()),
//	//	i3 = object_->pixel2index(px.right().down()),
//	//	i4 = object_->pixel2index(px.down());
//	//assert(i1>=0&&i1<object_->get_size().width*object_->get_size().height);
//	//assert(i2>=0&&i2<object_->get_size().width*object_->get_size().height);
//	//assert(i3>=0&&i3<object_->get_size().width*object_->get_size().height);
//	//assert(i4>=0&&i4<object_->get_size().width*object_->get_size().height);
//	//return
//	//	bilinear_interpolator2d_t
//	//		(
//	//			object_->data_[i1], object_->data_[i2],
//	//			object_->data_[i3], object_->data_[i4]
//	//		).calc_integral(p1, p2);
//}

#endif //LIBTOMO3D_OBJECT2D_H
