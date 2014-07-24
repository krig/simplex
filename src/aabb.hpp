#pragma once

// AABB collision / intersection / raycasting routines

struct AABB {
	vec3 lo; // min(x, y, z)
	vec3 hi; // max(x, y, z)
};

struct Ray {
	vec3 origin;
	vec3 dir;
};

namespace aabb {

	// create a bounding box from two points
	inline AABB create(const vec3& a, const vec3& b) {
		AABB ret;
		ret.lo.x = (std::min)(a.x, b.x);
		ret.lo.y = (std::min)(a.y, b.y);
		ret.lo.z = (std::min)(a.z, b.z);
		ret.hi.x = (std::max)(a.x, b.x);
		ret.hi.y = (std::max)(a.y, b.y);
		ret.hi.z = (std::max)(a.z, b.z);
		return ret;
	}

	// create a bounding box from a point and a radius
	inline AABB create(const vec3& p, float r) {
		AABB ret;
		ret.lo.x = p.x - r;
		ret.lo.y = p.y - r;
		ret.lo.z = p.z - r;
		ret.hi.x = p.x + r;
		ret.hi.y = p.y + r;
		ret.hi.z = p.z + r;
		return ret;
	}

	// create a bounding box from cloud of points
	inline AABB create(const vector<vec3>& points) {
		AABB ret;
		ret.lo = vec3(FLT_MAX, FLT_MAX, FLT_MAX);
		ret.hi = vec3(FLT_MIN, FLT_MIN, FLT_MIN);
		for (const auto& p : points) {
			ret.lo.x = (std::min)(ret.lo.x, p.x);
			ret.lo.y = (std::min)(ret.lo.y, p.y);
			ret.lo.z = (std::min)(ret.lo.z, p.z);
			ret.hi.x = (std::max)(ret.hi.x, p.x);
			ret.hi.y = (std::max)(ret.hi.y, p.y);
			ret.hi.z = (std::max)(ret.hi.z, p.z);
		}
		return ret;
	}

	inline bool inside(const AABB& box, const vec3& point) {
		return (point.x >= box.lo.x && point.x <= box.hi.x) &&
			(point.y >= box.lo.y && point.y <= box.hi.y) &&
			(point.z >= box.lo.z && point.z <= box.hi.z);
	}

	inline bool intersects(const AABB& box, const AABB& other) {
		// no intersection if one is completely < or > in any dimension
		return !((box.hi.x < other.lo.x) &&
		         (box.lo.x > other.hi.x) &&
		         (box.hi.y < other.lo.y) &&
		         (box.lo.y > other.hi.y) &&
		         (box.hi.z < other.lo.z) &&
		         (box.lo.z > other.hi.z));
	}

	// based on Fast Ray-Box Intersection
	// by Andrew Woo
	// from Graphics Gems, Academic Press, 1990
	// returns (hit, point)
	inline pair<bool, vec3> raycast(const AABB& box, const Ray& ray) {
		enum { Left, Right, Middle };
		bool inside = true;
		uint8_t quadrant[3];
		int which_plane;
		vec3 max_t;
		vec3 candidate_plane;

		// Find candidate planes; this loop can be avoided if
		// rays cast all from the eye(assume perpsective view)
		for (int i = 0; i < 3; ++i) {
			if(ray.origin[i] < box.lo[i]) {
				quadrant[i] = Left;
				candidate_plane[i] = box.lo[i];
				inside = false;
			}else if (ray.origin[i] > box.hi[i]) {
				quadrant[i] = Right;
				candidate_plane[i] = box.hi[i];
				inside = false;
			}else	{
				quadrant[i] = Middle;
			}
		}

		// Ray origin inside bounding box
		if(inside)	{
			return make_pair(true, ray.origin);
		}


		// Calculate T distances to candidate planes
		for (int i = 0; i < 3; ++i) {
			if (quadrant[i] != Middle && ray.dir[i] !=0.)
				max_t[i] = (candidate_plane[i] - ray.origin[i]) / ray.dir[i];
			else
				max_t[i] = -1.;
		}

		// Get largest of the maxT's for final choice of intersection
		which_plane = 0;
		for (int i = 1; i < 3; i++) {
			if (max_t[which_plane] < max_t[i])
				which_plane = i;
		}

		// Check final candidate actually inside box
		if (max_t[which_plane] < 0.)
			return make_pair(false, ray.origin);
		vec3 coord;
		for (int i = 0; i < 3; i++) {
			if (which_plane != i) {
				coord[i] = ray.origin[i] + max_t[which_plane] * ray.dir[i];
				if (coord[i] < box.lo[i] || coord[i] > box.hi[i])
					return make_pair(false, ray.origin);
			} else {
				coord[i] = candidate_plane[i];
			}
		}
		// ray hits box
		return make_pair(true, coord);
	}

}
