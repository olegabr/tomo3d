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

using namespace std;
using namespace boost;

struct hkl_t
{
	int h, k, l;
	explicit hkl_t(int h_ = 0, int k_ = 0, int l_ = 0) :
		h(h_), k(k_), l(l_) { normalize_(); }

	void normalize() {
		h = abs(h);
		k = abs(k);
		l = abs(l);

		{
			hkl_t _221[2] = { hkl_t(1,2,2), hkl_t(2,1,1) };
			if (std::find(_221, _221 + 2, *this) != (_221 + 2)) {
				*this = hkl_t(2,2,1);
			}
		}

		{
			hkl_t _211[2] = { hkl_t(1,1,2), hkl_t(1,2,1) };
			if (std::find(_211, _211 + 2, *this) != (_211 + 2)) {
				*this = hkl_t(2,1,1);
			}
		}

		{
			hkl_t _210[5] = { hkl_t(1,2,0), hkl_t(1,0,2), hkl_t(2,0,1), hkl_t(0,2,1), hkl_t(0,1,2) };
			if (std::find(_210, _210 + 5, *this) != (_210 + 5)) {
				*this = hkl_t(2,1,0);
			}
		}

		{
			hkl_t _110[2] = { hkl_t(1,0,1), hkl_t(0,1,1) };
			if (std::find(_110, _110 + 2, *this) != (_110 + 2)) {
				*this = hkl_t(1,1,0);
			}
		}

		{
			hkl_t _100[2] = { hkl_t(0,1,0), hkl_t(0,0,1) };
			if (std::find(_100, _100 + 2, *this) != (_100 + 2)) {
				*this = hkl_t(1,0,0);
			}
		}
	}

private :
	void fill_arr_(std::vector<int>& vecHKL, int x) {
		int nAbs = abs(x);
		if (nAbs > 0) {
			vecHKL.push_back(nAbs);
		}
	}

	void normalize_() {
		std::vector<int> vecHKL;
		fill_arr_(vecHKL, h);
		fill_arr_(vecHKL, k);
		fill_arr_(vecHKL, l);

		if (!vecHKL.empty()) {
			int nMin = *std::min_element(vecHKL.begin(), vecHKL.end());
			if (nMin > 1) {
				if (h % nMin == 0 && k % nMin == 0 && l % nMin == 0) {
					h /= nMin;
					k /= nMin;
					l /= nMin;
				}
			}

			//if (
			//	(h <= 0 && k <= 0 && l <= 0) ||
			//	(h < 0 && k < 0) ||
			//	(h < 0 && l < 0) ||
			//	(k < 0 && l < 0)
			//) {
			//	h = -h;
			//	k = -k;
			//	l = -l;
			//}
		}
	}
};

double operator* (hkl_t const& lh, hkl_t const& rh)
{
	return lh.h * rh.h + lh.k * rh.k + lh.l * rh.l;
}

double norm(hkl_t const& hkl)
{
	return std::sqrt(hkl * hkl);
}

bool operator== (hkl_t const& lh, hkl_t const& rh)
{
	return
		lh.h == rh.h &&
		lh.k == rh.k &&
		lh.l == rh.l;
}

bool hkl_vec_comp (hkl_t const& lh, hkl_t const& rh)
{
	// vector multiplication must be zero
	return
		lh.k * rh.l == rh.k * lh.l &&
		lh.h * rh.l == rh.h * lh.l &&
		lh.h * rh.k == rh.h * lh.k;
}

bool operator< (hkl_t const& lh, hkl_t const& rh)
{
	bool bRes = false;
	if (lh.h < rh.h) {
		bRes = true;
	} else if (lh.h == rh.h) {
		if (lh.k < rh.k) {
			bRes = true;
		} else if (lh.k == rh.k) {
			if (lh.l < rh.l) {
				bRes = true;
			}
		}
	}
	return bRes;
}

double calc_angle(hkl_t const& hkl1, hkl_t const& hkl2)
{
	double dCosAngle = hkl1 * hkl2 / (norm(hkl1) * norm(hkl2));
	double dAngleRadians = std::acos(dCosAngle);
	double dAngleDegrees = 180.0 - 180.0 * dAngleRadians / 3.14159265358979323F;
	return dAngleDegrees;
}

//istream& operator>> (istream& is, hkl_t& hkl)
//{
//	if (is) {
//		char c;
//		is >> c;
//		std::string s;
//		s += c;
//		hkl.h = boost::lexical_cast<int>(s);
//	}
//	if (is) {
//		char c;
//		is >> c;
//		std::string s;
//		s += c;
//		hkl.k = boost::lexical_cast<int>(s);
//	}
//	if (is) {
//		char c;
//		is >> c;
//		std::string s;
//		s += c;
//		hkl.l = boost::lexical_cast<int>(s);
//	}
//	return is;
//}

ostream& operator<< (ostream& os, hkl_t const& hkl)
{
	os << hkl.h << hkl.k << hkl.l;
	return os;
}


//typedef boost::array<hkl_t, 19> hkl_array;
typedef std::vector<hkl_t> hkl_array;

struct face_t
{
	char chName;
	hkl_t hkl;
};

ostream& operator<< (ostream& os, face_t const& f)
{
	os << f.chName << '\t' << f.hkl;
	return os;
}

struct face_data
{
	char chName1;
	char chName2;
	double dAngle;
};

istream& operator>> (istream& is, face_data& fd)
{
	is >> fd.chName1 >> fd.chName2 >> fd.dAngle;
	return is;
}

ostream& operator<< (ostream& os, face_data const& fa)
{
	os << fa.chName1 << ' ' << fa.chName2 << '\t' << fa.dAngle;
	return os;
}

struct face_hypo
{
	hkl_t hkl;
	double dAngleErr;
};

bool operator== (face_hypo const& lh, face_hypo const& rh)
{
	return
		lh.hkl == rh.hkl &&
		fabs(lh.dAngleErr - rh.dAngleErr) < 0.001;
}

bool face_hypo_hkl_less (face_hypo const& lh, face_hypo const& rh)
{
	return
		lh.hkl < rh.hkl;
}

bool face_hypo_hkl_comp (face_hypo const& lh, face_hypo const& rh)
{
	return
		lh.hkl == rh.hkl;
}

bool operator< (face_hypo const& lh, face_hypo const& rh)
{
	bool bRes = false;
	if (lh.hkl < rh.hkl) {
		bRes = true;
	} else if (lh.hkl == rh.hkl) {
		bRes = lh.dAngleErr < rh.dAngleErr;
	}
	return bRes;
}

bool hypo_err_less(face_hypo const& lh, face_hypo const& rh)
{
	return lh.dAngleErr < rh.dAngleErr;
}

//istream& operator>> (istream& is, face_hypo& fh)
//{
//	is >> fh.hkl >> fh.dAngleErr;
//	return is;
//}

ostream& operator<< (ostream& os, face_hypo const& fh)
{
	os << fh.hkl << '\t' << fh.dAngleErr;
	return os;
}

namespace {
	double g_dAngleErr = 0.0;

	hkl_array arrHKL;
	void calc_hkl_array()
	{
		const int nMax = 2;
		for(int h = -nMax; h <= nMax; ++h) {
			for(int k = -nMax; k <= nMax; ++k) {
				for(int l = -nMax; l <= nMax; ++l) {
					if (h == 0 && k == 0 && l == 0) {
						continue;
					}
					hkl_t face(h, k, l);
					arrHKL.push_back(face);
				}
			}
		}
		std::sort(arrHKL.begin(), arrHKL.end());
		arrHKL.erase(std::unique(arrHKL.begin(), arrHKL.end()), arrHKL.end());
	}

	vector< face_hypo > calc_hypos(double dAngle12)
	{
		vector< face_hypo > vecHypos;
		hkl_array::const_iterator iHKL = arrHKL.begin();
		hkl_array::const_iterator hklEnd = arrHKL.end();
		for (; iHKL != hklEnd; ++iHKL) {
			hkl_array::const_iterator jHKL = iHKL;
			for (++jHKL; jHKL != hklEnd; ++jHKL) {
				double dAngle = calc_angle(*iHKL, *jHKL);
				double dAngleErr = fabs(dAngle - dAngle12);
				if (dAngleErr < 0.001) {
					dAngleErr = 0.0;
				}
				if (dAngleErr <= g_dAngleErr) {
					face_hypo hypo;
					hypo.dAngleErr = dAngleErr;

					hypo.hkl = *iHKL;
					vecHypos.push_back(hypo);

					hypo.hkl = *jHKL;
					vecHypos.push_back(hypo);
				}
			}
		}

		std::sort(vecHypos.begin(), vecHypos.end());
		//std::cout << "before unique:\n";
		//std::copy(vecHypos.begin(), vecHypos.end(), std::ostream_iterator<face_hypo>(cout, "\n"));
		vecHypos.erase(std::unique(vecHypos.begin(), vecHypos.end(), &face_hypo_hkl_comp), vecHypos.end());
		//std::cout << "after unique:\n";
		//std::copy(vecHypos.begin(), vecHypos.end(), std::ostream_iterator<face_hypo>(cout, "\n"));
		std::sort(vecHypos.begin(), vecHypos.end());

		return vecHypos;
	}

	typedef std::map< char, std::vector<face_hypo> > CNamedHyposMap;
	void print_hypos(CNamedHyposMap const& mapFaceHypos)
	{
		CNamedHyposMap::const_iterator iterFaceHypos = mapFaceHypos.begin();
		CNamedHyposMap::const_iterator iterFaceHyposEnd = mapFaceHypos.end();
		for (; iterFaceHypos != iterFaceHyposEnd; ++iterFaceHypos) {
			cout
				<< iterFaceHypos->first
				<< "->\n";
			std::vector<face_hypo> vecHypos = iterFaceHypos->second;
			std::sort(vecHypos.begin(), vecHypos.end(), hypo_err_less);
			std::copy(vecHypos.begin(), vecHypos.end(), std::ostream_iterator<face_hypo>(cout, "\n"));
			cout << "\n";
		}
	}

	void add_hypos(char chName, CNamedHyposMap& mapFaceHypos, std::vector<face_hypo> const& vecHypos, face_data const& fd)
	{
		// 1) empty -> simple add
		// 2) nonempty -> intersect
		if (mapFaceHypos.find(chName) == mapFaceHypos.end()) {
			mapFaceHypos.insert(std::make_pair(chName, vecHypos));
		} else {
			std::vector<face_hypo> vecHypoIntersect;
			std::vector<face_hypo>::const_iterator iterMap = mapFaceHypos[chName].begin();
			std::vector<face_hypo>::const_iterator iterMapEnd = mapFaceHypos[chName].end();
			for (; iterMap != iterMapEnd; ++iterMap) {
				std::vector<face_hypo>::const_iterator iterHypos = vecHypos.begin();
				std::vector<face_hypo>::const_iterator iterHyposEnd = vecHypos.end();
				for (; iterHypos != iterHyposEnd; ++iterHypos) {
					if (iterHypos->hkl == iterMap->hkl) {
						if (iterMap->dAngleErr > iterHypos->dAngleErr) {
							vecHypoIntersect.push_back(*iterMap);
						} else {
							vecHypoIntersect.push_back(*iterHypos);
						}
					}
				}
			}

			if (vecHypoIntersect.empty()) {
				cout
					<< chName << " - intersection is empty.\n"
					<< "face_data: " << fd << "\n";
			}

			std::sort(vecHypoIntersect.begin(), vecHypoIntersect.end());
			vecHypoIntersect.erase(std::unique(vecHypoIntersect.begin(), vecHypoIntersect.end()), vecHypoIntersect.end());
			std::sort(vecHypoIntersect.begin(), vecHypoIntersect.end());

			mapFaceHypos[chName].swap(vecHypoIntersect);
		}
	}
}

// задаем пары граней одной буквой и измеренный угол между ними
// a b 125
// b c 120
// a c 139
int main(int argc, char* argv[])
try {
	calc_hkl_array();

	//std::copy(arrHKL.begin(), arrHKL.end(), std::ostream_iterator<hkl_t>(std::cout, "\n"));
	//std::cout << "---===================---\n";

	g_dAngleErr = 5.0;
	if (argc > 1) {
		g_dAngleErr = boost::lexical_cast<double>(argv[1]);
	}
	std::cout << "Max angle error: " << g_dAngleErr << "\n";

	std::map< char, std::vector<face_hypo> > mapFaceHypos;
	std::map< char, std::vector<face_data> > mapFaceData;
	while (cin && !cin.eof()) {
		face_data fd;
		cin >> fd;
		mapFaceData[fd.chName1].push_back(fd);
		mapFaceData[fd.chName2].push_back(fd);
		std::vector<face_hypo> vecHypos = calc_hypos(fd.dAngle);
		if (vecHypos.empty()) {
			std::cout << "Bad data: " << fd << "\n";
		} else {
			add_hypos(fd.chName1, mapFaceHypos, vecHypos, fd);
			add_hypos(fd.chName2, mapFaceHypos, vecHypos, fd);
		}
	}

	std::map< char, std::vector<face_data> >::const_iterator iterFaceData = mapFaceData.begin();
	std::map< char, std::vector<face_data> >::const_iterator iterFaceDataEnd = mapFaceData.end();
	for (; iterFaceData != iterFaceDataEnd; ++iterFaceData) {
		std::cout << iterFaceData->first << "->\n";
		std::copy(
			iterFaceData->second.begin(), iterFaceData->second.end(),
			std::ostream_iterator<face_data> (std::cout, "\n")
		);
		std::cout << "\n";
	}
	std::cout << "---===================---\n\n";

	std::map< char, std::vector<face_hypo> >::iterator iterMap = mapFaceHypos.begin();
	std::map< char, std::vector<face_hypo> >::iterator iterMapEnd = mapFaceHypos.end();
	for (; iterMap != iterMapEnd; ++iterMap) {
		std::vector<face_hypo>& vecFaceHypos = iterMap->second;
		if (!vecFaceHypos.empty()) {
			std::vector<face_hypo> vecFaceHyposRes;

			{
				std::vector<face_hypo>::iterator iterVec = vecFaceHypos.begin();
				std::vector<face_hypo>::iterator iterVecEnd = vecFaceHypos.end();
				for (; iterVec != iterVecEnd; ++iterVec) {
					iterVec->hkl.normalize();
				}
			}
			std::sort(vecFaceHypos.begin(), vecFaceHypos.end());

			std::vector<face_hypo>::iterator iterVec = vecFaceHypos.begin();
			std::vector<face_hypo>::iterator iterVecEnd = vecFaceHypos.end();
			face_hypo prevHypo = *iterVec;
			for (; iterVec != iterVecEnd; ++iterVec) {
				if (hkl_vec_comp(iterVec->hkl, prevHypo.hkl)) {
					if (iterVec->dAngleErr > prevHypo.dAngleErr) {
						prevHypo = *iterVec;
					}
				} else {
					vecFaceHyposRes.push_back(prevHypo);
					prevHypo = *iterVec;
				}
			}
			vecFaceHypos.swap(vecFaceHyposRes);
		}
	}

	print_hypos(mapFaceHypos);

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
