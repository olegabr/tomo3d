//util.h
/*
* the utilities
* (C) 2005 olegabr. All rights reserved.
*/
#ifndef LIBTOMO3D_UTIL_H
#define LIBTOMO3D_UTIL_H

// this type represents the size of 1d shadow object
typedef unsigned int size1d_t;
typedef int pixel1d;

typedef double point1d;

inline in_grid(pixel1d i, size1d_t sz)
{
	return i>=0 && i<static_cast<pixel1d>(sz);
}

namespace util {
double round(double d);
}

#endif //LIBTOMO3D_UTIL_H
