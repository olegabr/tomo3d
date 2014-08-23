//transforms_ex.h
/*
* the transforms from object to shadow2d and vice versa
* (C) 2005 olegabr. All rights reserved.
*/
#ifndef TOMO3D_TRANSFORMS_H
#define TOMO3D_TRANSFORMS_H

#include <boost/shared_ptr.hpp>
#include <vector>

class shadow2d;
class object3d;
struct deviation_t;
boost::shared_ptr<shadow2d> bin_by_pow2(shadow2d& sh, int n);
void vary_alignment(shadow2d const& sh_, object3d const& object, int pxnum, shadow2d& sh1, shadow2d& sh2, std::vector<deviation_t>&);
void apply_deviations(shadow2d& sh, std::vector<deviation_t>& vdev, int bin_by2);

#endif //TOMO3D_TRANSFORMS_H
