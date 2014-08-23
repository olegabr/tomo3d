//transforms.h
/*
* the transforms from object to shadow2d and vice versa
* (C) 2005 olegabr. All rights reserved.
*/
#ifndef LIBTOMO3D_TRANSFORMS_H
#define LIBTOMO3D_TRANSFORMS_H

class object3d;
class shadow2d;
class filtered_shadow2d;
class projection2d;

shadow2d& project(const object3d& object, shadow2d& shdw);
shadow2d& project_overwrite(const object3d& object, shadow2d& shdw);

object3d& backproject(const filtered_shadow2d& filtered_shadow, object3d& object, double betha = 1.);
object3d& backproject_overwrite(const filtered_shadow2d& filtered_shadow, object3d& object, double betha = 1.);

object3d& backproject(const projection2d& fp, object3d& object, double betha = 1.);
object3d& backproject_overwrite(const projection2d& fp, object3d& object, double betha = 1.);

shadow2d& normalize(shadow2d& sh);
shadow2d& align_to_gravity_center(shadow2d& sh);

#endif //LIBTOMO3D_TRANSFORMS_H
