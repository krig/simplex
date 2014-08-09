#include "common.hpp"
#include "render.hpp"
#include "geo.hpp"
#include "mesher.hpp"

namespace geo {

	vector<cube_vert> make_cube_vertices(const vec3& size, bool invert_normals) {

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

		vector<cube_vert> verts = {
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

		if (invert_normals) {
			for (size_t i = 0; i < verts.size(); i += 3) {
				swap(verts[i + 1].pos, verts[i + 2].pos);
				swap(verts[i + 1].texcoord, verts[i + 2].texcoord);
				verts[i].normal = -glm::normalize(verts[i].pos);
				verts[i+1].normal = -glm::normalize(verts[i+1].pos);
				verts[i+2].normal = -glm::normalize(verts[i+2].pos);
			}
		}

		return verts;
	}

	vector<cube_vert> make_cone_vertices(float height, float radius, int subdivisions, bool invert_normals) {
		vector<cube_vert> verts;
		verts.reserve(3 * subdivisions);
		//verts.push_back

		float dsd = 1.f / (float)subdivisions;
		float dangle = TWOPI / (float)subdivisions;
		vec3 p0(0.f, height, 0.f);
		for (int i = 0; i < subdivisions; ++i) {
			float fi = (float)i;
			vec3 p1(radius * cosf(dangle * fi), 0.f, radius * sinf(dangle * fi));
			vec3 p2(radius * cosf(dangle * fi + dangle), 0.f, radius * sinf(dangle * fi + dangle));

			vec3 mp = p1 + (p2 - p1) * 0.5f;
			mp.y = height * 0.5f;

			if (invert_normals) {
				verts.push_back({ p0, -glm::normalize(mp), vec2(0.f, 1.f) });
				verts.push_back({ p1, -glm::normalize(p1), vec2(dsd * fi, 0.f) });
				verts.push_back({ p2, -glm::normalize(p2), vec2(dsd * fi + dsd, 0.f) });
			} else {
				verts.push_back({ p0, glm::normalize(mp), vec2(0.f, 1.f) });
				verts.push_back({ p2, glm::normalize(p2), vec2(dsd * fi + dsd, 0.f) });
				verts.push_back({ p1, glm::normalize(p1), vec2(dsd, 0.f) });
			}
		}
		return verts;
	}

	void make_cube(geometry* obj, const vec3& size, bool invert_normals) {
		obj->array.gen();
		obj->array.bind();
		VBO buffer;
		buffer.gen();
		buffer.bind(GL_ARRAY_BUFFER);
		buffer.data(GL_ARRAY_BUFFER, make_cube_vertices(size, invert_normals));
		obj->array.pointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
		obj->array.pointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (const void*)(3*sizeof(float)));
		obj->array.pointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (const void*)(6*sizeof(float)));
		obj->array.attrib(3, vec3(1.f, 1.f, 1.f));
		obj->array.unbind();
		obj->nelements = 6 * 2 * 3;
	}

	void make_cone(geometry* obj, float height, float radius, int subdivisions, bool invert_normals) {
		obj->array.gen();
		obj->array.bind();
		VBO buffer;
		buffer.gen();
		buffer.bind(GL_ARRAY_BUFFER);
		buffer.data(GL_ARRAY_BUFFER, make_cone_vertices(height, radius, subdivisions, invert_normals));
		obj->array.pointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
		obj->array.pointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (const void*)(3*sizeof(float)));
		obj->array.pointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (const void*)(6*sizeof(float)));
		obj->array.attrib(3, vec3(1.f, 1.f, 1.f));
		obj->array.unbind();
		obj->nelements = 3 * subdivisions;
	}

	void make_plane(geometry* obj, float segment_size, int segments) {

		vector<cube_vert> verts;

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

		obj->array.gen();
		obj->array.bind();
		VBO buffer;
		buffer.gen();
		buffer.bind(GL_ARRAY_BUFFER);
		buffer.data(GL_ARRAY_BUFFER, verts);
		obj->array.pointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
		obj->array.pointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (const void*)(3*sizeof(float)));
		obj->array.pointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (const void*)(6*sizeof(float)));
		obj->array.unbind();
		obj->nelements = segments * segments * 6;
	}

}
