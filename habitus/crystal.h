//crystal.h
/*
* the crystal abstraction
* it is simulated by a collection of faces
* (C) 2005 olegabr. All rights reserved.
*/
#ifndef TOMO3D_CRYSTAL_H
#define TOMO3D_CRYSTAL_H

#include <vector>

class crystal
{
public :
	crystal() {}

private :
	std::vector<face> m_vecFaces;
};

#endif //TOMO3D_CRYSTAL_H
