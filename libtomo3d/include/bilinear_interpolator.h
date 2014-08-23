//bilinear_interpolator.h
/*
* gives values between two known points
* (C) 2005 olegabr. All rights reserved.
*/
#ifndef LIBTOMO3D_BILINEAR_INTERPOLATOR_H
#define LIBTOMO3D_BILINEAR_INTERPOLATOR_H

/*
билинейный интерполятор.
по значениям в углах квадрата со стороной 1
выдает значения во всех внутренних точках.
левый верхний угол = (0,0).
квадрат разбит на два треугольника:
1) left_top, right_top, left_bottom
2) left_top, left_bottom, right_bottom
соответственно две плоскости, по ним и интерполируем.
*/
struct bilinear_interpolator_t
{
	typedef double value_type;

	bilinear_interpolator_t
	(
		value_type left_top,
		value_type right_top,
		value_type left_bottom,
		value_type right_bottom
	) :
		left_top_(left_top), right_top_(right_top),
		left_bottom_(left_bottom), right_bottom_(right_bottom) {}

	bilinear_interpolator_t(const bilinear_interpolator_t& other) :
		left_top_(other.left_top_), right_top_(other.right_top_),
		left_bottom_(other.left_bottom_), right_bottom_(other.right_bottom_) {}

	bilinear_interpolator_t& operator = (const bilinear_interpolator_t& other)
	{
		left_top_ = other.left_top_;
		right_top_ = other.right_top_;
		left_bottom_ = other.left_bottom_;
		right_bottom_ = other.right_bottom_;
		return *this;
	}
	value_type operator ()(const point2d& p) const
	{
		if (p.x<p.y)//left triangle
			return (right_bottom_ - left_bottom_)*p.x + (left_bottom_ - left_top_)*p.y + left_top_;
		//else right triangle
		return (right_top_ - left_top_)*p.x + (right_bottom_ - right_top_)*p.y + left_top_;
	}

private :
	value_type left_top_, right_top_, left_bottom_, right_bottom_;
};

#endif //LIBTOMO3D_BILINEAR_INTERPOLATOR_H
