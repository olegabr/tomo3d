//object3d.h
/*
* the 3d object
* (C) 2005 olegabr. All rights reserved.
*/
#ifndef LIBTOMO3D_OBJECT3D_H
#define LIBTOMO3D_OBJECT3D_H

#include <cassert>
#include <vector>
#include <string>
#include <map>

#include "object2d.h"
#include "size3d.h"
#include "pixel3d.h"
#include "point3d.h"

#include <boost/iterator/iterator_facade.hpp>

// это исходный 3D объект.
// система координат в центре объекта (X/2;Y/2;Z/2)
class object3d
{
public :
	typedef object2d::value_type value_type;
	typedef std::vector<object2d> data_type;

	object3d();//for debug
	~object3d();
	object3d(size3d_t sz, point3d::value_type point_size = 1);
	//coping of this object is very expensive
	object3d(const object3d&);
	object3d& operator =(const object3d&);
	void clear();
	void swap(object3d&) /*throw()*/;

	//construct from image
	object3d(const std::string& image_name);
	void save_to_image(const std::string& image_name) const;
	void load_from_image(const std::string& dir);

	value_type norm2() const;
	//void eliminate_negativeness();

	void add_scaled_diff(object3d const& diff, double betha = 1.0);
	void assign_scaled_diff(object3d const& diff, double betha = 1.0);

	point3d::value_type get_point_size() const { return point_size_; }
	size3d_t get_size() const { return size_; }

	class iterator
		: public boost::iterator_facade<
			object3d::iterator,
			object2d,
			boost::random_access_traversal_tag
		>
	{
		enum {END = -1};
	public:
		iterator() : position_(END), object_(0) {}
		iterator(object3d* object) : object_(object), position_(0) {}
		iterator(const iterator& i) : object_(i.object_), position_(i.position_) {}
		point2d::value_type get_position() const;

	private :
		friend class boost::iterator_core_access;

		object2d& dereference() const;
		void increment();
		bool equal(const iterator&) const;
		void advance (iterator::difference_type n);
		iterator::difference_type distance_to(const iterator&) const;

		pixel1d position_;
		object3d* object_;
	};
	friend class iterator;

	iterator begin() { return iterator(this); }
	iterator end  () { return iterator(); }

	//iterate through slices
	class const_iterator
		: public boost::iterator_facade<
			object3d::const_iterator,
			object2d const,
			boost::random_access_traversal_tag
		>
	{
		enum {END = -1};
	public:
		const_iterator() : position_(END), object_(0) {}
		const_iterator(const object3d* object) : object_(object), position_(0) {}
		const_iterator(const const_iterator& i) : object_(i.object_), position_(i.position_) {}

		point2d::value_type get_position() const;
		pixel1d get_position_index() const;

	private :
		friend class boost::iterator_core_access;

		object2d const& dereference() const;
		void increment();
		bool equal(const const_iterator&) const;
		void advance(object3d::const_iterator::difference_type);
		iterator::difference_type distance_to(const const_iterator&) const;

		pixel1d position_;
		const object3d* object_;
	};
	friend class const_iterator;

	const_iterator begin() const { return const_iterator(this); }
	const_iterator end  () const { return const_iterator(); }

	value_type& operator ()(const pixel3d&);
	value_type operator ()(const pixel3d&) const;

	pixel3d point2pixel(const point3d&) const;

private :

	point3d::value_type get_depth_ () const { return (size_.depth  - 1)*get_point_size(); }
	point3d::value_type get_zmin_() const { return -0.5*get_depth_(); }

	std::pair<value_type, value_type> get_min_max_() const;

	//variables:
	data_type data_;
	size3d_t size_; //число точек по каждой из координат
	point3d::value_type point_size_; //размеры точки в nm
};

inline object2d& object3d::iterator::dereference() const
{
	assert(in_grid(position_, object_->size_.depth));
	return object_->data_[position_];
}

inline void object3d::iterator::increment()
{
	assert(in_grid(position_, object_->size_.depth));
	if (position_ == static_cast<pixel1d>(object_->size_.depth-1))
		position_ = object3d::iterator::END;
	else ++position_;
}

inline bool object3d::iterator::equal(const object3d::iterator& i) const
{
	return position_ == i.position_;
}

inline void object3d::iterator::advance (object3d::iterator::difference_type n)
{
	assert(in_grid(position_, object_->size_.depth));
	position_ += n;
	if (position_ > static_cast<pixel1d>(object_->size_.depth-1))
		position_ = object3d::iterator::END;
}

inline object3d::iterator::difference_type
object3d::iterator::distance_to(const object3d::iterator& i) const
{
	//assert(object_->size_.depth == i.object_->size_.depth);
	if (position_ == object3d::iterator::END)
		if (i.position_ == object3d::iterator::END)
			return 0;
		else
			return
				-static_cast<object3d::iterator::difference_type>
					(i.object_->size_.depth - i.position_);

	if (i.position_ == object3d::iterator::END)
		return object_->size_.depth - position_;
	return i.position_ - position_;
}

inline point2d::value_type object3d::iterator::get_position() const
{
	assert(in_grid(position_, object_->size_.depth));
	return object_->get_zmin_() + position_*object_->get_point_size();
}

inline object2d const& object3d::const_iterator::dereference() const
{
	assert(in_grid(position_, object_->size_.depth));
	return object_->data_[position_];
}

inline void object3d::const_iterator::increment()
{
	assert(in_grid(position_, object_->size_.depth));
	if (position_ == static_cast<pixel1d>(object_->size_.depth-1))
		position_ = object3d::const_iterator::END;
	else ++position_;
}

inline bool object3d::const_iterator::equal(const object3d::const_iterator& i) const
{
	return position_ == i.position_;
}

inline void
object3d::const_iterator::advance(object3d::const_iterator::difference_type n)
{
	assert(in_grid(position_, object_->size_.depth));
	position_ += n;
	if (position_ > static_cast<pixel1d>(object_->size_.depth-1))
		position_ = object3d::const_iterator::END;
}

inline object3d::const_iterator::difference_type
object3d::const_iterator::distance_to(const object3d::const_iterator& i) const
{
	//assert(object_->size_.depth == i.object_->size_.depth);
	if (position_ == object3d::const_iterator::END)
		if (i.position_ == object3d::const_iterator::END)
			return 0;
		else
			return
				-static_cast<object3d::const_iterator::difference_type>
					(i.object_->size_.depth - i.position_);

	if (i.position_ == object3d::const_iterator::END)
		return object_->size_.depth - position_;
	return i.position_ - position_;
}

inline point2d::value_type object3d::const_iterator::get_position() const
{
	assert(in_grid(position_, object_->size_.depth));
	return object_->get_zmin_() + position_*object_->get_point_size();
}

inline pixel1d object3d::const_iterator::get_position_index() const
{
	assert(in_grid(position_, object_->size_.depth));
	return position_;
}

#endif //LIBTOMO3D_OBJECT3D_H
