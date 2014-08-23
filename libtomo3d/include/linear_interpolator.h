//linear_interpolator.h
/*
* gives values between two known points
* (C) 2005 olegabr. All rights reserved.
*/
#ifndef LIBTOMO3D_LINEAR_INTERPOLATOR_H
#define LIBTOMO3D_LINEAR_INTERPOLATOR_H

/*
линейный интерполятор.
по значениям на границах отрезка длины 1
выдает значения во всех внутренних точках.
значение координаты на левой границе = 0
*/
struct linear_interpolator_t
{
	typedef double value_type;

	linear_interpolator_t
	(
		value_type left,
		value_type right
	) : left_(left), right_(right) {}

	linear_interpolator_t(const linear_interpolator_t& other)
		: left_(other.left_), right_(other.right_) {}

	linear_interpolator_t& operator = (const linear_interpolator_t& other)
	{
		left_ = other.left_;
		right_ = other.right_;
		return *this;
	}
	/*
	* returns the linear interpolation of the point between
	* the left and right points.
	* p := [0,1]
	*/
	value_type operator ()(point1d p) const
	{
		return (right_ - left_)*p + left_;
	}
	value_type get_left() const { return left_; }
	value_type get_right() const { return right_; }
private :
	value_type left_, right_;
};

#endif //LIBTOMO3D_LINEAR_INTERPOLATOR_H
