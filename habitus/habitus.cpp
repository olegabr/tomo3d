//habitus.cpp
/*
* habitus detection from 3D density map of some particle
* obtained by tomography reconstruction
* (C) 2005 olegabr. All rights reserved.
*/

#include <Magick++.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
using std::cout; using std::endl;

#include <cmath>
using std::sqrt;

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#include <object3d.h>
#include "transforms_ex.h"
#include "grid2d.h"
#include "angle.h"

namespace {
	void usage()
	{
		std::cout << "habitus images_directory\n";
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		usage();
		return 0;
	}
	try {
		Magick::InitializeMagick(argv[0]);

		// load the 3D object
		object3d particle(argv[1]);

		// calculate or get from user the threshold value
		// points with values less then the threshold value
		// are not considered
		object3d::value_type threshold = calc_mean(particle);

		// get from user, or set by default the lambda value
		object3d::value_type lambda = 1;

		// calculate the particle's center
		// for example, the mass center
		point3d center = calc_gravity_center(particle);

		// calculate Rmin and Rmax values
		// Rmin - is the maximum radius of continuos density
		// without holes
		// Rmax - is the minimum radius of sphere that contains all
		// points with non-zero (note threshold) values
		point3d::value_type Rmin = calc_min_radius(particle, center, threshold);
		point3d::value_type Rmax = calc_max_radius(particle, center, Rmin, threshold);

		// construct a 2D grid in theta-phi space with d(theta), d(phi) steps
		typedef grid2d<angle_t, angle_t> grid_t;

		const angle_t theta_min(0);
		const angle_t theta_max(90);
		const angle_t d_theta(1);
		grid_t::xgrid_type grid_theta(theta_min, theta_max, d_theta);

		const angle_t phi_min(0);
		const angle_t phi_max(360);
		const angle_t d_phi(1);
		grid_t::ygrid_type grid_phi(phi_min, phi_max, d_phi);

		grid_t grid(grid_theta, grid_phi);

		// for each node of this grid
		std::vector<std::pair<grid_t::value_type, double> > values(grid.size());
		for (grid_t::const_iterator gi = grid.begin(); gi != grid.end(); ++gi)
		{
			// find the L: the sum between Lmax and L planes is
			// equal to the sum between Lmin and L planes.
			// Lmin and Lmax are found during this process.
			// as an additional result we have a M, Sfull and h values here.
			// M is a sum of masses between Lmin and Lmax
			// h is a distance between them
			// Sfull is an area of a figure given by intersection of
			// the Lmin plane with the particle's shape

			// S is an area of a figure given by intersection of
			// the L plane with the particle's shape

			// calc the M' = M / (h * Sfull)

			// calc the S' = S / Sfull

			// calc the functional:
			// G = (M' - lambda * (S' - 1)) / (1 + lambda)

			typedef grid1d<point3d::value_type> lgrid_t;
			lgrid_t lgrid(Rmin, Rmax, particle.get_point_size());

			std::vector<std::pair<lgrid_t::value_type, double> > lvalues(lgrid.size());
			for (lgrid_t::const_iterator lgi = lgrid.begin(); lgi != lgrid.end(); ++lgi)
			{
			}

			double G, S;
			calc_G_S(particle, Rmin, Rmax, *gi, G, S);
			values.push_back(std::make_pair(*gi, G));
			//const point3d::value_type dL = particle.get_point_size();
			//for (point3d::value_type L(Rmin); L <= Rmax; L += dL)
			//{
			//}
		}

		// select points where a local minimum is reached

		// for each local minimum:
			// repeat the described procedure for new grid with
			// fine-tuned d(theta), d(phi) in the area of this local minimum
	}
	catch (boost::bad_lexical_cast &ex) {
		cout << ex.what() << endl;
	}
	catch (Magick::Exception &ex) {
		cout << ex.what() << endl;
	}
	catch (std::exception &e) {
		cout << e.what() << endl;
	}
	catch (...) {
		cout << "Unknown exception occured." << endl;
	}

	return 0;
}
