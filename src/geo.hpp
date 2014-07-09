#pragma once

struct cube_vert {
	vec3 pos;
	vec3 normal;
};

std::vector<cube_vert> make_cube_vertices(const vec3& size);

