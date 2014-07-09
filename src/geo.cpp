#include "common.hpp"
#include "render.hpp"
#include "geo.hpp"

namespace geo {

std::vector<cube_vert> make_cube_vertices(const vec3& size) {

	vec3 corners[8] = {
		{-1.f,-1.f,-1.f}, // bottom
		{ 1.f,-1.f,-1.f},
		{-1.f,-1.f, 1.f},
		{ 1.f,-1.f, 1.f},
		{-1.f, 1.f,-1.f}, // top
		{-1.f, 1.f, 1.f},
		{ 1.f, 1.f,-1.f},
		{ 1.f, 1.f, 1.f}
	};

	// scale by size
	for (int i = 0; i < 8; ++i)
		corners[i] *= size;

	std::vector<cube_vert> verts = {
		// bottom
		{ corners[0], { 0.f, -1.f, 0.f } },
		{ corners[1], { 0.f, -1.f, 0.f } },
		{ corners[2], { 0.f, -1.f, 0.f } },
		{ corners[1], { 0.f, -1.f, 0.f } },
		{ corners[3], { 0.f, -1.f, 0.f } },
		{ corners[2], { 0.f, -1.f, 0.f } },

		// top
		{ corners[4], { 0.f, 1.f, 0.f } },
		{ corners[5], { 0.f, 1.f, 0.f } },
		{ corners[6], { 0.f, 1.f, 0.f } },
		{ corners[6], { 0.f, 1.f, 0.f } },
		{ corners[5], { 0.f, 1.f, 0.f } },
		{ corners[7], { 0.f, 1.f, 0.f } },

		// front
		{ corners[2], { 0.f, 0.f, 1.f } },
		{ corners[3], { 0.f, 0.f, 1.f } },
		{ corners[5], { 0.f, 0.f, 1.f } },
		{ corners[3], { 0.f, 0.f, 1.f } },
		{ corners[7], { 0.f, 0.f, 1.f } },
		{ corners[5], { 0.f, 0.f, 1.f } },

		// back
		{ corners[0], { 0.f, 0.f,-1.f } },
		{ corners[4], { 0.f, 0.f,-1.f } },
		{ corners[1], { 0.f, 0.f,-1.f } },
		{ corners[1], { 0.f, 0.f,-1.f } },
		{ corners[4], { 0.f, 0.f,-1.f } },
		{ corners[6], { 0.f, 0.f,-1.f } },

		// left
		{ corners[2], {-1.f, 0.f, 0.f } },
		{ corners[4], {-1.f, 0.f, 0.f } },
		{ corners[0], {-1.f, 0.f, 0.f } },
		{ corners[2], {-1.f, 0.f, 0.f } },
		{ corners[5], {-1.f, 0.f, 0.f } },
		{ corners[4], {-1.f, 0.f, 0.f } },

		// right
		{ corners[3], { 1.f, 0.f, 0.f } },
		{ corners[1], { 1.f, 0.f, 0.f } },
		{ corners[6], { 1.f, 0.f, 0.f } },
		{ corners[3], { 1.f, 0.f, 0.f } },
		{ corners[6], { 1.f, 0.f, 0.f } },
		{ corners[7], { 1.f, 0.f, 0.f } },
	};

	return verts;
}

}
