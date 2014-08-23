//projection2d.h
/*
* the projection2d class
* (C) 2005 olegabr. All rights reserved.
*/
#ifndef LIBTOMO3D_PROJECTION2D_H
#define LIBTOMO3D_PROJECTION2D_H

#include "projection1d.h"
#include "point2d.h"
#include "size2d.h"
#include "pixel2d.h"
#include "angle.h"
#include "deviation.h"
#include <vector>
#include <map>

/*
 тень это проекция обьекта.
 реальные данные всегда собираются приемником с фиксированным разрешением
 отсюда два параметра: число точек и размер точки
 задаются эти парамтры в конструкторе тени

 система координат тени фиксирована: нуль в центре

 размер этого объекта не может быть изменен после его создания
*/
class projection2d
{
public :
	typedef projection1d::value_type value_type;
	typedef std::vector<projection1d> data_type;

	typedef data_type::iterator iterator;
	typedef data_type::const_iterator const_iterator;

	friend class projection2d_tester;
	void draw_cube(double d); //debug
	projection2d();
	projection2d
		(
			const size2d_t& size,
			point2d::value_type point_size/*nm*/,
			const angle_t& angle
		);
	projection2d(const projection2d& other);
	projection2d& operator =(const projection2d& other);
	~projection2d();
	void clear();
	//void bin_by_pow2(int n);

	//g = f-g
	projection2d& assign_difference (const projection2d& f);
	projection2d& assign_and_rotate (const projection2d&, const angle_t&);
	projection2d& assign_and_deviate(const projection2d&, const deviation_t&);

	void load(const std::string& img);
	void save_to_image(std::string img_name, std::pair<value_type, value_type> const &min_max) const;

	//in place filtration
	void ro_filtrate();
	// the dot product of two 2d projections
	friend value_type
		operator *(const projection2d&, const projection2d&);

	value_type get_value(const point2d& p) const;
	projection1d& get_slice(pixel1d w);

	point2d::value_type get_point_size() const { return point_size_; }
	angle_t get_angle() const { return angle_; }
	size2d_t get_size() const { return size_; }
	
	point2d get_mass_center() const;
	value_type get_mass() const;

	iterator begin() { return data_.begin(); }
	iterator end  () { return data_.end(); }
	const_iterator begin() const { return data_.begin(); }
	const_iterator end  () const { return data_.end(); }

	void swap(projection2d&);
	point2d pixel2point(const pixel2d& p) const { return pixel2point_(p); }

	friend std::ostream& operator <<(std::ostream& os, const projection2d& sh);

	std::pair<value_type, value_type> get_min_max_() const;

private :
	value_type get_value_(const pixel2d& px) const;
	//value_type get_value_(const point2d& p) const;
	void set_value_(const pixel2d& p, value_type value);
	//void add_value_(const point2d& p, value_type value);

	point2d::value_type get_xmin_() const { return -0.5*(size_.width - 1)*point_size_; }
	point2d::value_type get_ymin_() const { return  -0.5*(size_.height - 1)*point_size_; }

	pixel2d index2pixel_(std::size_t i) const;
	pixel2d point2pixel_(const point2d&) const;
	point2d pixel2point_(const pixel2d&) const;
	bool is_integral_(const deviation_t&) const;

	size2d_t size_;
	point2d::value_type point_size_;
	data_type data_;
	angle_t angle_;
};

#endif //LIBTOMO3D_PROJECTION2D_H
