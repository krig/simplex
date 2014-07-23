#include "common.hpp"
#include "render.hpp"
#include "geo.hpp"

namespace geo {

	std::vector<cube_vert> make_cube_vertices(const vec3& size, bool skybox) {

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
			{ corners[0], { 0.f, -1.f, 0.f }, { 0.f, 0.f } },
			{ corners[1], { 0.f, -1.f, 0.f }, { 1.f, 0.f } },
			{ corners[2], { 0.f, -1.f, 0.f }, { 0.f, 1.f } },
			{ corners[1], { 0.f, -1.f, 0.f }, { 1.f, 0.f } },
			{ corners[3], { 0.f, -1.f, 0.f }, { 1.f, 1.f } },
			{ corners[2], { 0.f, -1.f, 0.f }, { 0.f, 1.f } },

			// top
			{ corners[4], { 0.f, 1.f, 0.f }, { 0.f, 0.f } },
			{ corners[5], { 0.f, 1.f, 0.f }, { 0.f, 1.f } },
			{ corners[6], { 0.f, 1.f, 0.f }, { 1.f, 0.f } },
			{ corners[6], { 0.f, 1.f, 0.f }, { 1.f, 0.f } },
			{ corners[5], { 0.f, 1.f, 0.f }, { 0.f, 1.f } },
			{ corners[7], { 0.f, 1.f, 0.f }, { 1.f, 1.f } },

			// front
			{ corners[2], { 0.f, 0.f, 1.f }, { 0.f, 0.f } },
			{ corners[3], { 0.f, 0.f, 1.f }, { 1.f, 0.f } },
			{ corners[5], { 0.f, 0.f, 1.f }, { 0.f, 1.f } },
			{ corners[3], { 0.f, 0.f, 1.f }, { 1.f, 0.f } },
			{ corners[7], { 0.f, 0.f, 1.f }, { 1.f, 1.f } },
			{ corners[5], { 0.f, 0.f, 1.f }, { 0.f, 1.f } },

			// back
			{ corners[0], { 0.f, 0.f,-1.f }, { 0.f, 0.f } },
			{ corners[4], { 0.f, 0.f,-1.f }, { 0.f, 1.f } },
			{ corners[1], { 0.f, 0.f,-1.f }, { 1.f, 0.f } },
			{ corners[1], { 0.f, 0.f,-1.f }, { 1.f, 0.f } },
			{ corners[4], { 0.f, 0.f,-1.f }, { 0.f, 1.f } },
			{ corners[6], { 0.f, 0.f,-1.f }, { 1.f, 1.f } },

			// left
			{ corners[2], {-1.f, 0.f, 0.f }, { 0.f, 1.f } },
			{ corners[4], {-1.f, 0.f, 0.f }, { 1.f, 0.f } },
			{ corners[0], {-1.f, 0.f, 0.f }, { 0.f, 0.f } },
			{ corners[2], {-1.f, 0.f, 0.f }, { 0.f, 1.f } },
			{ corners[5], {-1.f, 0.f, 0.f }, { 1.f, 1.f } },
			{ corners[4], {-1.f, 0.f, 0.f }, { 1.f, 0.f } },

			// right
			{ corners[3], { 1.f, 0.f, 0.f }, { 0.f, 1.f } },
			{ corners[1], { 1.f, 0.f, 0.f }, { 0.f, 0.f } },
			{ corners[6], { 1.f, 0.f, 0.f }, { 1.f, 0.f } },
			{ corners[3], { 1.f, 0.f, 0.f }, { 0.f, 1.f } },
			{ corners[6], { 1.f, 0.f, 0.f }, { 1.f, 0.f } },
			{ corners[7], { 1.f, 0.f, 0.f }, { 1.f, 1.f } },
		};

		if (skybox) {
			for (size_t i = 0; i < verts.size(); i += 3) {
				std::swap(verts[i + 1].pos, verts[i + 2].pos);
				std::swap(verts[i + 1].texcoord, verts[i + 2].texcoord);
				verts[i].normal = -glm::normalize(verts[i].pos);
				verts[i+1].normal = -glm::normalize(verts[i+1].pos);
				verts[i+2].normal = -glm::normalize(verts[i+2].pos);
			}
		}

		return verts;
	}

	void cube::make(const vec3& size, bool skybox) {
		array.gen();
		array.bind();
		buffer.gen();
		buffer.bind(GL_ARRAY_BUFFER);
		buffer.data(GL_ARRAY_BUFFER, make_cube_vertices(size, skybox));
		array.pointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
		array.pointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (const void*)(3*sizeof(float)));
		array.pointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (const void*)(6*sizeof(float)));
		array.attrib(3, vec3(1.f, 1.f, 1.f));
		array.unbind();
	}


	void plane::make() {

		std::vector<cube_vert> verts;

		for (int z = -segments/2; z < segments/2; ++z) {
			for (int x = -segments/2; x < segments/2; ++x) {
				float xp = x * segment_size;
				float zp = z * segment_size;

				verts.emplace_back(cube_vert { { xp - segment_size, 0.f, zp - segment_size}, { 0.f, 1.f, 0.f }, { 0.f, 0.f } });
				verts.emplace_back(cube_vert { { xp - segment_size, 0.f, zp }, { 0.f, 1.f, 0.f }, { 0.f, 1.f } });
				verts.emplace_back(cube_vert { { xp, 0.f, zp - segment_size}, { 0.f, 1.f, 0.f }, { 1.f, 0.f } });

				verts.emplace_back(cube_vert { { xp, 0.f, zp - segment_size}, { 0.f, 1.f, 0.f }, { 1.f, 0.f } });
				verts.emplace_back(cube_vert { { xp - segment_size, 0.f, zp }, { 0.f, 1.f, 0.f }, { 0.f, 1.f } });
				verts.emplace_back(cube_vert { { xp, 0.f, zp }, { 0.f, 1.f, 0.f }, { 1.f, 1.f } });
			}
		}

		array.gen();
		array.bind();
		buffer.gen();
		buffer.bind(GL_ARRAY_BUFFER);
		buffer.data(GL_ARRAY_BUFFER, verts);
		array.pointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
		array.pointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (const void*)(3*sizeof(float)));
		array.pointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (const void*)(6*sizeof(float)));
		array.unbind();
	}

}
