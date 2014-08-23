//projection1d.h
/*
* the projection1d class
* (C) 2004 olegabr. All rights reserved.
*/
#ifndef LIBTOMO3D_PROJECTION1D_H
#define LIBTOMO3D_PROJECTION1D_H

#include "util.h"
#include <vector>
#include <map>

/*
 тень это проекция обьекта.
 реальные данные всегда собираются приемником с фиксированным разрешением
 отсюда два параметра: число точек и размер точки
 задаются эти парамтры в конструкторе тени

 система координат тени фиксирована: нуль в центре + offset
*/
class projection1d
{
public :
	typedef double value_type;
	typedef std::vector<value_type> data_type;
	typedef data_type::iterator iterator;
	typedef data_type::const_iterator const_iterator;

	//friend class projection1d_tester;

	explicit projection1d(size1d_t size = 0, point1d point_size = 0);
	~projection1d() {}
	projection1d(const projection1d& other);
	projection1d& operator =(const projection1d& other);
	void clear();

	//g = f-g
	projection1d& assign_difference (const projection1d& f);

	//in place filtration
	void ro_filtrate();
	// the dot product of two 1d projections
	friend value_type
		operator *(const projection1d&, const projection1d&);

	void add_value(pixel1d i, value_type value);
	void add_value(point1d R, value_type value);

	void set_value(pixel1d i, value_type value);
	void set_value(point1d R, value_type value);

	value_type get_value(pixel1d i) const;

	point1d get_point_size() const { return point_size_; }
	size1d_t get_size() const { return size_; }
	// gt means greater then
	point1d get_point_gt(point1d l) const;

	friend std::ostream& operator <<(std::ostream& os, const projection1d& sh);

	iterator begin() { return data_.begin(); }
	iterator end  () { return data_.end(); }
	const_iterator begin() const { return data_.begin(); }
	const_iterator end  () const { return data_.end(); }

	std::pair<value_type, value_type> get_min_max_() const;

private :
	point1d get_length_() const { return (size_ - 1)*point_size_; }
	//point1d get_Rmax_() const { return 0.5*get_length_(); }
	point1d get_Rmin_() const { return -0.5*get_length_(); }

	pixel1d point2index_(point1d R) const;

	size1d_t size_;   //число точек в этой дискретной ф.-ии
	point1d point_size_; //размер точки
	data_type data_;
};

#endif //LIBTOMO3D_PROJECTION1D_H
