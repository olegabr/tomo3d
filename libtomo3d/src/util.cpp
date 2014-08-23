//util.cpp
/*
* the utilities implementation
* (C) 2005 olegabr. All rights reserved.
*/
#include "util.h"
#include <cmath>

double util::round(double d)
{
	double value = std::floor(d);
	return d - value < 0.5 ? value : value+1;
}
