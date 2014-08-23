//deviation.cpp
/*
* the deviation class impl
* (C) 2005 olegabr. All rights reserved.
*/
#include <iostream>
#include "deviation.h"

std::ostream& operator <<(std::ostream& os, const deviation_t& dev)
{
	os
		<< '('
		<< dev.offset << ','
		//<< dev.rotation_angle << ','
		<< dev.magnification_factor
		<< ')';
	return os;
}

bool operator == (const deviation_t& lh, const deviation_t& rh)
{
	return
		lh.offset == rh.offset &&
		//lh.rotation_angle == rh.rotation_angle &&
		lh.magnification_factor == rh.magnification_factor;
}
