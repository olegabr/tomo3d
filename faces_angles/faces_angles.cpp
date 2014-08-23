// faces_angles.cpp
/*
 * Calculates all angles between crystal grids
 * (C) 2006 olegabr. All rights reserved.
*/

#include <iostream>
using std::cout; using std::endl;

#include <cmath>
using std::sqrt;

#include <vector>
using std::vector;

#include <map>
using std::multimap;

#include <utility>
using std::pair; using std::make_pair;

#include <boost/array.hpp>
#include <boost/lexical_cast.hpp>

#include <point3d.h>
typedef point3d face_t;

typedef boost::array<face_t, 19> hkl_array;

struct face_pair
{
	face_t face1;
	face_t face2;
	friend bool operator == (face_pair const& lh, face_pair const& rh) {
		return
			lh.face1 == rh.face1 &&
			lh.face2 == rh.face2;
	}
	friend bool operator != (face_pair const& lh, face_pair const& rh) {
		return !(lh == rh);
	}
};

struct face_triple
{
	face_t face1;
	face_t face2;
	face_t face3;
};

namespace {
	double calc_angle(face_t const& face1, face_t const& face2)
	{
		double dCosAngle = face1 * face2 / (norm(face1) * norm(face2));
		double dAngleRadians = std::acos(dCosAngle);
		double dAngleDegrees = 180.0 - 180.0 * dAngleRadians / 3.14159265358979323F;
		return dAngleDegrees;
	}

	void print_faces(hkl_array const& arrHKL)
	{
		hkl_array::const_iterator iHKL = arrHKL.begin();
		hkl_array::const_iterator hklEnd = arrHKL.end();
		for (; iHKL != hklEnd; ++iHKL) {
			hkl_array::const_iterator jHKL = iHKL;
			for (++jHKL; jHKL != hklEnd; ++jHKL) {
				cout
					<< iHKL->x << iHKL->y << iHKL->z
					<< '\t'
					<< jHKL->x << jHKL->y << jHKL->z
					<< '\t'
					<< calc_angle(*iHKL, *jHKL) << "\n";
			}
		}
	}

	vector< face_pair >
	calc_faces(hkl_array const& arrHKL, double dAngle12, double dAngleErrorMin, double dAngleErrorMax)
	{
		multimap< double/*angle error*/, face_pair > mapFaces;

		hkl_array::const_iterator iHKL = arrHKL.begin();
		hkl_array::const_iterator hklEnd = arrHKL.end();
		for (; iHKL != hklEnd; ++iHKL) {
			hkl_array::const_iterator jHKL = iHKL;
			for (++jHKL; jHKL != hklEnd; ++jHKL) {
				double dAngle = calc_angle(*iHKL, *jHKL);
				double dAngleDelta = fabs(dAngle - dAngle12);
				if (dAngleDelta >= dAngleErrorMin && dAngleDelta <= dAngleErrorMax) {
					face_pair pr;
					pr.face1 = *iHKL;
					pr.face2 = *jHKL;
					mapFaces.insert(make_pair(dAngleDelta, pr));
				}
			}
		}

		vector< face_pair > vecFaces;
		vecFaces.reserve(mapFaces.size());

		multimap< double, face_pair >::const_iterator iterMap = mapFaces.begin();
		multimap< double, face_pair >::const_iterator iterMapEnd = mapFaces.end();
		for(; iterMap != iterMapEnd; ++iterMap) {
			vecFaces.push_back(iterMap->second);
		}

		return vecFaces;
	}

	void print_faces_pairs(vector< face_pair > const& vecFaces, double dAngle12)
	{
		vector< face_pair >::const_iterator iHKL = vecFaces.begin();
		vector< face_pair >::const_iterator hklEnd = vecFaces.end();
		for (; iHKL != hklEnd; ++iHKL) {
			face_t f1 = iHKL->face1;
			face_t f2 = iHKL->face2;
			double dAngle = calc_angle(f1, f2);
			double dErr = fabs(dAngle - dAngle12);
			dErr = dErr >= 0.01 ? dErr : 0.0;
			cout
				<< f1.x << f1.y << f1.z
				<< '\t'
				<< f2.x << f2.y << f2.z
				<< '\t'
				<< dAngle
				<< '\t'
				<< dErr
				<< "\n";
		}
	}

	void print_faces_within_error_bounds(hkl_array const& arrHKL, double dAngle12, double dAngleErrorMin, double dAngleErrorMax)
	{
		vector< face_pair > vecFaces;
		vecFaces = calc_faces(arrHKL, dAngle12, dAngleErrorMin, dAngleErrorMax);
		if (!vecFaces.empty()) {
			cout << "Angle error is in range [" << dAngleErrorMin << ", " << dAngleErrorMax << "]\n";
			print_faces_pairs(vecFaces, dAngle12);
			cout << "\n";
		}
	}

	void print_faces_triples(vector< face_triple > const& vecFaces, double dAngle12, double dAngle23)
	{
		vector< face_triple >::const_iterator iHKL = vecFaces.begin();
		vector< face_triple >::const_iterator hklEnd = vecFaces.end();
		for (; iHKL != hklEnd; ++iHKL) {
			face_t f1 = iHKL->face1;
			face_t f2 = iHKL->face2;
			face_t f3 = iHKL->face3;
			double dAngle1 = calc_angle(f1, f2);
			double dAngle2 = calc_angle(f2, f3);
			double dAngle3 = calc_angle(f3, f1);

			double dErr1 = fabs(dAngle1 - dAngle12);
			dErr1 = dErr1 >= 0.01 ? dErr1 : 0.0;

			double dErr2 = fabs(dAngle2 - dAngle23);
			dErr2 = dErr2 >= 0.01 ? dErr2 : 0.0;

			cout
				<< f1.x << f1.y << f1.z
				<< '\t'
				<< f2.x << f2.y << f2.z
				<< '\t'
				<< f3.x << f3.y << f3.z
				<< '\t'
				<< dAngle1
				<< '\t'
				<< dErr1
				<< '\t'
				<< dAngle2
				<< '\t'
				<< dErr2
				<< '\t'
				<< dAngle3
				<< "\n";
		}
	}

	void print_faces_within_error_bounds(
		hkl_array const& arrHKL,
		double dAngle12,
		double dAngle23,
		double dAngleErrorMin, double dAngleErrorMax
	)
	{
		vector< face_pair > vecFaces12;
		vecFaces12 = calc_faces(arrHKL, dAngle12, dAngleErrorMin, dAngleErrorMax);

		vector< face_pair > vecFaces23;
		vecFaces23 = calc_faces(arrHKL, dAngle23, dAngleErrorMin, dAngleErrorMax);

		if (!vecFaces12.empty() && !vecFaces23.empty()) {
			vector<face_triple> vecFaceTriples;

			vector< face_pair >::const_iterator iterFacePair12 = vecFaces12.begin();
			vector< face_pair >::const_iterator iterFacePair12End = vecFaces12.end();
			for (; iterFacePair12 != iterFacePair12End; ++iterFacePair12) {
				vector< face_pair >::const_iterator iterFacePair23 = vecFaces23.begin();
				vector< face_pair >::const_iterator iterFacePair23End = vecFaces23.end();
				for (; iterFacePair23 != iterFacePair23End; ++iterFacePair23) {
					if (*iterFacePair12 != *iterFacePair23) {
						if (iterFacePair12->face1 == iterFacePair23->face1) {
							face_triple triple;
							triple.face1 = iterFacePair12->face2;
							triple.face2 = iterFacePair12->face1;
							triple.face3 = iterFacePair23->face2;
							vecFaceTriples.push_back(triple);
						} else if (iterFacePair12->face1 == iterFacePair23->face2) {
							face_triple triple;
							triple.face1 = iterFacePair12->face2;
							triple.face2 = iterFacePair12->face1;
							triple.face3 = iterFacePair23->face1;
							vecFaceTriples.push_back(triple);
						} else if (iterFacePair12->face2 == iterFacePair23->face1) {
							face_triple triple;
							triple.face1 = iterFacePair12->face1;
							triple.face2 = iterFacePair12->face2;
							triple.face3 = iterFacePair23->face2;
							vecFaceTriples.push_back(triple);
						} else if (iterFacePair12->face2 == iterFacePair23->face2) {
							face_triple triple;
							triple.face1 = iterFacePair12->face1;
							triple.face2 = iterFacePair12->face2;
							triple.face3 = iterFacePair23->face1;
							vecFaceTriples.push_back(triple);
						}
					}
				}
			}


			cout << "Angle error is in range [" << dAngleErrorMin << ", " << dAngleErrorMax << "]\n";
			print_faces_triples(vecFaceTriples, dAngle12, dAngle23);
			cout << "\n";
		}
	}

	void print_faces_triples(vector< face_triple > const& vecFaces, double dAngle12, double dAngle23, double dAngle31)
	{
		vector< face_triple >::const_iterator iHKL = vecFaces.begin();
		vector< face_triple >::const_iterator hklEnd = vecFaces.end();
		for (; iHKL != hklEnd; ++iHKL) {
			face_t f1 = iHKL->face1;
			face_t f2 = iHKL->face2;
			face_t f3 = iHKL->face3;
			double dAngle1 = calc_angle(f1, f2);
			double dAngle2 = calc_angle(f2, f3);
			double dAngle3 = calc_angle(f3, f1);

			double dErr1 = fabs(dAngle1 - dAngle12);
			dErr1 = dErr1 >= 0.01 ? dErr1 : 0.0;

			double dErr2 = fabs(dAngle2 - dAngle23);
			dErr2 = dErr2 >= 0.01 ? dErr2 : 0.0;

			double dErr3 = fabs(dAngle3 - dAngle31);
			dErr3 = dErr3 >= 0.01 ? dErr3 : 0.0;

			cout
				<< f1.x << f1.y << f1.z
				<< '\t'
				<< f2.x << f2.y << f2.z
				<< '\t'
				<< f3.x << f3.y << f3.z
				<< '\t'
				<< dAngle1
				<< '\t'
				<< dErr1
				<< '\t'
				<< dAngle2
				<< '\t'
				<< dErr2
				<< '\t'
				<< dAngle3
				<< '\t'
				<< dErr3
				<< "\n";
		}
	}

	void print_faces_within_error_bounds(
		hkl_array const& arrHKL,
		double dAngle12,
		double dAngle23,
		double dAngle31,
		double dAngleErrorMin, double dAngleErrorMax
	)
	{
		vector< face_pair > vecFaces12;
		vecFaces12 = calc_faces(arrHKL, dAngle12, dAngleErrorMin, dAngleErrorMax);

		vector< face_pair > vecFaces23;
		vecFaces23 = calc_faces(arrHKL, dAngle23, dAngleErrorMin, dAngleErrorMax);

		if (!vecFaces12.empty() && !vecFaces23.empty()) {
			vector<face_triple> vecFaceTriples;

			vector< face_pair >::const_iterator iterFacePair12 = vecFaces12.begin();
			vector< face_pair >::const_iterator iterFacePair12End = vecFaces12.end();
			for (; iterFacePair12 != iterFacePair12End; ++iterFacePair12) {
				vector< face_pair >::const_iterator iterFacePair23 = vecFaces23.begin();
				vector< face_pair >::const_iterator iterFacePair23End = vecFaces23.end();
				for (; iterFacePair23 != iterFacePair23End; ++iterFacePair23) {
					if (*iterFacePair12 != *iterFacePair23) {
						if (iterFacePair12->face1 == iterFacePair23->face1) {
							double dAngle = calc_angle(iterFacePair12->face2, iterFacePair23->face2);
							double dErr = fabs(dAngle - dAngle31);
							dErr = dErr >= 0.01 ? dErr : 0;
							if (dErr >= dAngleErrorMin && dErr <= dAngleErrorMax) {
								face_triple triple;
								triple.face1 = iterFacePair12->face2;
								triple.face2 = iterFacePair12->face1;
								triple.face3 = iterFacePair23->face2;
								vecFaceTriples.push_back(triple);
							}
						} else if (iterFacePair12->face1 == iterFacePair23->face2) {
							double dAngle = calc_angle(iterFacePair12->face2, iterFacePair23->face1);
							double dErr = fabs(dAngle - dAngle31);
							dErr = dErr >= 0.01 ? dErr : 0;
							if (dErr >= dAngleErrorMin && dErr <= dAngleErrorMax) {
								face_triple triple;
								triple.face1 = iterFacePair12->face2;
								triple.face2 = iterFacePair12->face1;
								triple.face3 = iterFacePair23->face1;
								vecFaceTriples.push_back(triple);
							}
						} else if (iterFacePair12->face2 == iterFacePair23->face1) {
							double dAngle = calc_angle(iterFacePair12->face1, iterFacePair23->face2);
							double dErr = fabs(dAngle - dAngle31);
							dErr = dErr >= 0.01 ? dErr : 0;
							if (dErr >= dAngleErrorMin && dErr <= dAngleErrorMax) {
								face_triple triple;
								triple.face1 = iterFacePair12->face1;
								triple.face2 = iterFacePair12->face2;
								triple.face3 = iterFacePair23->face2;
								vecFaceTriples.push_back(triple);
							}
						} else if (iterFacePair12->face2 == iterFacePair23->face2) {
							double dAngle = calc_angle(iterFacePair12->face1, iterFacePair23->face1);
							double dErr = fabs(dAngle - dAngle31);
							dErr = dErr >= 0.01 ? dErr : 0;
							if (dErr >= dAngleErrorMin && dErr <= dAngleErrorMax) {
								face_triple triple;
								triple.face1 = iterFacePair12->face1;
								triple.face2 = iterFacePair12->face2;
								triple.face3 = iterFacePair23->face1;
								vecFaceTriples.push_back(triple);
							}
						}
					}
				}
			}

			if (!vecFaceTriples.empty()) {
				cout << "Angle error is in range [" << dAngleErrorMin << ", " << dAngleErrorMax << "]\n";
				print_faces_triples(vecFaceTriples, dAngle12, dAngle23, dAngle31);
				cout << "\n";
			}
		}
	}

	void usage()
	{
		cout
			<< "up to 3 angles between 3 faces (in degrees):\n"
			   "f1 f2 f3 => angle12 [angle23 [angle31]]"
			<< endl;
	}
}

using std::fixed;

int main(int argc, char* argv[])
try {
	hkl_array arrHKL =
	{
		face_t(1.0, 1.0, 1.0),

		face_t(1.0, 1.0, 0.0),
		face_t(1.0, 0.0, 1.0),
		face_t(0.0, 1.0, 1.0),

		face_t(1.0, 0.0, 0.0),
		face_t(0.0, 0.0, 1.0),
		face_t(0.0, 1.0, 0.0),

		face_t(1.0, 1.0, 2.0),
		face_t(2.0, 1.0, 1.0),
		face_t(1.0, 2.0, 1.0),

		face_t(2.0, 1.0, 0.0),
		face_t(1.0, 2.0, 0.0),
		face_t(1.0, 0.0, 2.0),
		face_t(2.0, 0.0, 1.0),
		face_t(0.0, 2.0, 1.0),
		face_t(0.0, 1.0, 2.0),

		face_t(1.0, 2.0, 2.0),
		face_t(2.0, 1.0, 2.0),
		face_t(2.0, 2.0, 1.0)
	};

	//cout.precision(2);
	//cout << fixed;

	switch(argc) {
		case 1 :
			print_faces(arrHKL);
			break;
		case 2 : // angle12 in degrees
			{
				double dAngle12 = boost::lexical_cast<double>(argv[1]);
				print_faces_within_error_bounds(arrHKL, dAngle12, 0.0, 1.0);
				print_faces_within_error_bounds(arrHKL, dAngle12, 1.0, 2.0);
				print_faces_within_error_bounds(arrHKL, dAngle12, 2.0, 3.0);
				print_faces_within_error_bounds(arrHKL, dAngle12, 3.0, 4.0);
				print_faces_within_error_bounds(arrHKL, dAngle12, 4.0, 5.0);
				print_faces_within_error_bounds(arrHKL, dAngle12, 5.0, 6.0);
			}
			break;
		case 3 : // angle12 angle23
			{
				double dAngle12 = boost::lexical_cast<double>(argv[1]);
				double dAngle23 = boost::lexical_cast<double>(argv[2]);
				print_faces_within_error_bounds(arrHKL, dAngle12, dAngle23, 0.0, 5.0);
				////print_faces_within_error_bounds(arrHKL, dAngle12, dAngle23, 0.0, 1.0);
				//print_faces_within_error_bounds(arrHKL, dAngle12, dAngle23, 1.0, 2.0);
				//print_faces_within_error_bounds(arrHKL, dAngle12, dAngle23, 2.0, 3.0);
				//print_faces_within_error_bounds(arrHKL, dAngle12, dAngle23, 3.0, 4.0);
				//print_faces_within_error_bounds(arrHKL, dAngle12, dAngle23, 4.0, 5.0);
				//print_faces_within_error_bounds(arrHKL, dAngle12, dAngle23, 5.0, 6.0);
			}
			break;
		case 4 : // angle12 angle23 angle31
			{
				double dAngle12 = boost::lexical_cast<double>(argv[1]);
				double dAngle23 = boost::lexical_cast<double>(argv[2]);
				double dAngle31 = boost::lexical_cast<double>(argv[3]);
				print_faces_within_error_bounds(arrHKL, dAngle12, dAngle23, dAngle31, 0.0, 5.0);
				//print_faces_within_error_bounds(arrHKL, dAngle12, dAngle23, dAngle31, 0.0, 1.0);
				//print_faces_within_error_bounds(arrHKL, dAngle12, dAngle23, dAngle31, 1.0, 2.0);
				//print_faces_within_error_bounds(arrHKL, dAngle12, dAngle23, dAngle31, 2.0, 3.0);
				//print_faces_within_error_bounds(arrHKL, dAngle12, dAngle23, dAngle31, 3.0, 4.0);
				//print_faces_within_error_bounds(arrHKL, dAngle12, dAngle23, dAngle31, 4.0, 5.0);
				//print_faces_within_error_bounds(arrHKL, dAngle12, dAngle23, dAngle31, 5.0, 6.0);
			}
			break;
		default :
			usage();
			break;
	}

	return 0;
}
catch (std::exception &e) {
	cout << e.what() << endl;
	return 1;
}
catch (...) {
	cout << "Unknown exception occured." << endl;
	return 1;
}
