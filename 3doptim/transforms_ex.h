//transforms_ex.h
/*
* the transforms from object to shadow2d and vice versa
* (C) 2005 olegabr. All rights reserved.
*/
#ifndef _3DOPTIM_TRANSFORMS_H
#define _3DOPTIM_TRANSFORMS_H

class object3d;
class shadow2d;
struct point3d;
shadow2d& create_mask(const shadow2d& sh, shadow2d& mask);
shadow2d& apply_mask(const shadow2d& mask, shadow2d& sh);
point3d calc_center(const shadow2d& sh);
void black_threshold(shadow2d& sh, double threshould);
object3d& apply_filter(const object3d& obj, const point3d& center, object3d& result);
object3d& filter_object(object3d& object);

#endif //_3DOPTIM_TRANSFORMS_H
