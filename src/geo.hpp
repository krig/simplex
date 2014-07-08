#pragma once

struct cube_vert {
	glm::vec3 pos;
	glm::vec2 uv;
};

std::vector<cube_vert> make_cube_vertices(const glm::vec3& size);

