//transforms_ex.h
/*
* the transforms from object to shadow2d and vice versa
* (C) 2005 olegabr. All rights reserved.
*/
#ifndef TOMO3D_TRANSFORMS_H
#define TOMO3D_TRANSFORMS_H

#include <vector>
#include <point3d.h>

class object3d;
point3d calc_gravity_center(object3d& obj);

// for all points between two faces do some functor
template<class Func>
void apply(face const& face1, face const& face2, Func f);

#endif //TOMO3D_TRANSFORMS_H
