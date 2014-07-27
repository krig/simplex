/*
 * worley noise
 * based on:
 *
 * wnoise.cpp
 *
 * Copyright (C) 1998, Matt Pharr <mmp@graphics.stanford.edu>
 *
 * This software is placed in the public domain and is provided as is
 * without express or implied warranty.
 *
 * Basic implementation of Steve Worley's noise function; see proceedings
 * of SIGGRAPH 96.
 */

#include "common.hpp"
#include <assert.h>
#include <alloca.h>

inline double lerp(double t, double a, double b) {
	return (1. - t) * a + t * b;
}

inline double distance_squared(const glm::dvec3 &a, const glm::dvec3 &b) {
	return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z);
}

inline double frac(double d) {
	return d >= 0. ? d - int(d) : frac(-d);
}

inline double sqr(double a) {
	return a * a;
}

inline int max(int a, int b) {
	return a > b ? a : b;
}

namespace {
/* return a uniform random number from 0 to 1. */
	double uniformRandom() {
		return drand48();
	}

/* Seed the random number generator.  After beeing seeded with a giveny
   value, the random number generator *must* always then return the same
   sequence of values when uniformRandom() above is called. */

	inline void
	seedRandom(int x, int y, int z)
	{
		long seed = (x << 16) + (y << 8) + z;
		srand48(seed);
	}


	void doVoxel(int x, int y, int z, const glm::dvec3 &p, double density,
	             int num, double *dist, glm::dvec3 *vec);
}

	void worley_noise_impl(const glm::dvec3 &pp, double density, int num, double *dist2, glm::dvec3 *vec)
	{
		// hack, but easier than handling points that are exactly at negative
		// integer latice-points correctly.
		glm::dvec3 p(pp.x + 1e-7, pp.y + 1e-7, pp.z + 1e-7);

		int i;
		int xx = floor(p.x), yy = floor(p.y), zz = floor(p.z);
		double *dd = (double *)alloca(num * sizeof(double));
		assert(num < 10);

		double *d = dist2;
		if (!d)
			d = dd;

		// initialize the distances to the num closest points to be
		// enormous so that anything we find is closer.
		for (i = 0; i < num; ++i)
			d[i] = HUGE_VAL;

		// first check voxel the point is in
		doVoxel(xx, yy, zz, p, density, num, d, vec);

		// squared distance to the closest point that we're interested in; the
		// remaining voxels around us don't need to be checked if their
		// squared distance from p is greater than this.
		double d2 = d[num - 1];

		// check each of the voxels that share a face with the
		// point's voxel, if they're close enough to possibly
		// make a difference

		// squared distance to the voxel in the +x direction
		double dpx2 = p.x >= 0. ? sqr(1. - frac(p.x)) : sqr(frac(p.x));
		if (dpx2 < d2) {
			doVoxel(xx+1, yy, zz, p, density, num, d, vec);
			d2 = d[num - 1];
		}

		// -x
		double dnx2 = p.x >= 0. ? sqr(frac(p.x)) : sqr(1. - frac(p.x));
		if (dnx2 < d2) {
			doVoxel(xx-1, yy, zz, p, density, num, d, vec);
			d2 = d[num - 1];
		}

		// +y
		double dpy2 = p.y >= 0. ? sqr(1. - frac(p.y)) : sqr(frac(p.y));
		if (dpy2 < d2) {
			doVoxel(xx, yy+1, zz, p, density, num, d, vec);
			d2 = d[num - 1];
		}

		// -y
		double dny2 = p.y >= 0. ? sqr(frac(p.y)) : sqr(1. - frac(p.y));
		if (dny2 < d2) {
			doVoxel(xx, yy-1, zz, p, density, num, d, vec);
			d2 = d[num - 1];
		}

		// +z
		double dpz2 = p.z >= 0. ? sqr(1. - frac(p.z)) : sqr(frac(p.z));
		if (dpz2 < d2) {
			doVoxel(xx, yy, zz+1, p, density, num, d, vec);
			d2 = d[num - 1];
		}

		// -z
		double dnz2 = p.z >= 0. ? sqr(frac(p.z)) : sqr(1. - frac(p.z));
		if (dnz2 < d2) {
			doVoxel(xx, yy, zz-1, p, density, num, d, vec);
			d2 = d[num - 1];
		}

		// finally check the remaining adjacent voxels
		for (i = -1; i <= 1; ++i) {
			for (int j = -1; j <= 1; ++j) {
				for (int k = -1; k <= 1; ++k) {
					// don't check the ones we already did above
					if (abs(i) + abs(j) + abs(k) <= 1)
						continue;

					// find squared distance to voxel
					double vd2 = 0;
					if (i < 0)
						vd2 += dnx2;
					else if (i > 0)
						vd2 += dpx2;
					if (j < 0)
						vd2 += dny2;
					else if (j > 0)
						vd2 += dpy2;
					if (k < 0)
						vd2 += dnz2;
					else if (k > 0)
						vd2 += dpz2;

					// and check it if it's close enough to matter
					if (vd2 < d2) {
						doVoxel(xx + i, yy + j, zz + k, p, density, num, d, vec);
						d2 = d[num - 1];
					}
				}
			}
		}
	}


namespace {

// for a given voxel, generate a set of noise points in the voxel and
// check their distance against the point p.  Keep the _num_ closest
// distances (squared) in dist2[] and the _num_ vectors to those points
// in vec[].  Obviously this is a little redundant.

	void doVoxel(int x, int y, int z, const glm::dvec3 &p, double density,
	             int num, double *dist2, glm::dvec3 *vec)
	{
		seedRandom(x, y, z);

		// how many points in this voxel?
		int npts = max(1, int(.5 + lerp(uniformRandom(), 
		                                density / 2., 2. * density)));

		for (int i = 0; i < npts; ++i) {
			glm::dvec3 noisePt(x + uniformRandom(),
			                   y + uniformRandom(),
			                   z + uniformRandom());

			double d2 = distance_squared(p, noisePt);

			for (int j = 0; j < num; ++j)
				// is this point closer than some previous closest point?
				if (d2 < dist2[j]) {
					// push the following points forward in the array
					// to make room for this one
					for (int k = j; k < num - 1; ++k) {
						dist2[k+1] = dist2[k];
						if (vec)
							vec[k+1] = vec[k];
					}
					dist2[j] = d2;
					if (vec) {
						vec[j].x = noisePt.x - p.x;
						vec[j].y = noisePt.y - p.y;
						vec[j].z = noisePt.z - p.z;
					}
					break;
				}

		}

		// sanity
		for (int j = 0; j < num-1; ++j)
			assert(dist2[j] <= dist2[j+1]);
	}
}
