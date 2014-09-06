#include "common.hpp"
#include "render.hpp"
#include "geo.hpp"
#include "mesher.hpp"

#include <deque>

namespace geo {

	vector<cube_vert> make_cube_vertices(const vec3& size, bool invert_normals) {

		vec3 corners[8] = {
			{-0.5f,-0.5f,-0.5f}, // bottom
			{ 0.5f,-0.5f,-0.5f},
			{-0.5f,-0.5f, 0.5f},
			{ 0.5f,-0.5f, 0.5f},
			{-0.5f, 0.5f,-0.5f}, // top
			{-0.5f, 0.5f, 0.5f},
			{ 0.5f, 0.5f,-0.5f},
			{ 0.5f, 0.5f, 0.5f}
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

	namespace {
		void add_face(std::deque<vec3>& faceverts, vec3 a, vec3 b, vec3 c) {
			faceverts.push_back(a);
			faceverts.push_back(b);
			faceverts.push_back(c);
		}

		void add_face(vector<cube_vert>& tris, vec3 a, vec3 b, vec3 c) {
			tris.push_back({a, a, vec2(0, a.y * 0.5f + 0.5f)});
			tris.push_back({b, b, vec2(0, b.y * 0.5f + 0.5f)});
			tris.push_back({c, c, vec2(0, c.y * 0.5f + 0.5f)});
		}

		void initial_icosahedron(std::deque<vec3>& faceverts) {
			vector<vec3> verts(12);

			double theta = 26.56505117707799 * PI / 180.0; // refer paper for theta value

			double stheta = std::sin(theta);
			double ctheta = std::cos(theta);

			verts[0] = vec3(0.0f, 0.0f, -1.0f); // the lower vertex

			// the lower pentagon
			double phi = PI / 5.0;
			for (int i = 1; i < 6; ++i) {
				verts[i] = vec3(ctheta * std::cos(phi), ctheta * std::sin(phi), -stheta);
				phi += 2.0 * PI / 5.0;
			}

			// the upper pentagon
			phi = 0.0;
			for (int i = 6; i < 11; ++i) {
				verts[i] = vec3(ctheta * std::cos(phi), ctheta * std::sin(phi), stheta);
				phi += 2.0 * PI / 5.0;
			}

			verts[11] = vec3(0.0f, 0.0f, 1.0f); // the upper vertex

			add_face(faceverts, verts[0], verts[2], verts[1]);
			add_face(faceverts, verts[0], verts[3], verts[2]);
			add_face(faceverts, verts[0], verts[4], verts[3]);
			add_face(faceverts, verts[0], verts[5], verts[4]);
			add_face(faceverts, verts[0], verts[1], verts[5]);
			add_face(faceverts, verts[1], verts[2], verts[7]);
			add_face(faceverts, verts[2], verts[3], verts[8]);
			add_face(faceverts, verts[3], verts[4], verts[9]);
			add_face(faceverts, verts[4], verts[5], verts[10]);
			add_face(faceverts, verts[5], verts[1], verts[6]);
			add_face(faceverts, verts[1], verts[7], verts[6]);
			add_face(faceverts, verts[2], verts[8], verts[7]);
			add_face(faceverts, verts[3], verts[9], verts[8]);
			add_face(faceverts, verts[4], verts[10], verts[9]);
			add_face(faceverts, verts[5], verts[6], verts[10]);
			add_face(faceverts, verts[6], verts[7], verts[11]);
			add_face(faceverts, verts[7], verts[8], verts[11]);
			add_face(faceverts, verts[8], verts[9], verts[11]);
			add_face(faceverts, verts[9], verts[10], verts[11]);
			add_face(faceverts, verts[10], verts[6], verts[11]);
		}

		void subdivide(std::deque<vec3>& faceverts) {
			// for each subdivision pass, pop N/3 verts
			// off, push the subdivided verts back
			size_t nfaces = faceverts.size() / 3;
			for (size_t i = 0; i < nfaces; ++i) {
				vec3 v1 = faceverts.front(); faceverts.pop_front();
				vec3 v2 = faceverts.front(); faceverts.pop_front();
				vec3 v3 = faceverts.front(); faceverts.pop_front();
				vec3 v4 = glm::normalize(v1 + v2);
				vec3 v5 = glm::normalize(v2 + v3);
				vec3 v6 = glm::normalize(v3 + v1);
				faceverts.push_back(v1);
				faceverts.push_back(v4);
				faceverts.push_back(v6);
				faceverts.push_back(v4);
				faceverts.push_back(v2);
				faceverts.push_back(v5);
				faceverts.push_back(v6);
				faceverts.push_back(v5);
				faceverts.push_back(v3);
				faceverts.push_back(v6);
				faceverts.push_back(v4);
				faceverts.push_back(v5);
			}
		}

		void make_sphere_verts(vector<cube_vert>& tris, float radius, int subdivisions, bool invert_normals) {
			std::deque<vec3> faceverts;
			initial_icosahedron(faceverts);

			// subdivide!
			for (int sd = 0; sd < subdivisions; ++sd) {
				subdivide(faceverts);
			}

			tris.clear();
			tris.reserve(faceverts.size());
			for (vec3 v : faceverts)
				tris.push_back({v, v, vec2(0, v.y * 0.5f + 0.5f)});

			for (size_t i = 0; i < tris.size(); ++i) {
				tris[i].pos *= radius;
			}

			if (invert_normals) {
				for (size_t i = 0; i < tris.size(); i += 3) {
					swap(tris[i + 1].pos, tris[i + 2].pos);
					swap(tris[i + 1].texcoord, tris[i + 2].texcoord);
					swap(tris[i + 1].normal, tris[i + 2].normal);
					tris[i].normal = -tris[i].normal;
					tris[i+1].normal = -tris[i+1].normal;
					tris[i+2].normal = -tris[i+2].normal;
				}
			}
		}
	}



	void make_icosahedron(geometry* obj, float radius, bool invert_normals) {
		obj->array.gen();
		obj->array.bind();
		VBO buffer;
		buffer.gen();
		buffer.bind(GL_ARRAY_BUFFER);
		vector<cube_vert> tris;
		make_sphere_verts(tris, radius, 0, invert_normals);
		buffer.data(GL_ARRAY_BUFFER, tris);
		obj->array.pointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
		obj->array.pointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (const void*)(3*sizeof(float)));
		obj->array.pointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (const void*)(6*sizeof(float)));
		obj->array.unbind();
		obj->nelements = tris.size();
	}

	void make_sphere(geometry* obj, float radius, int subdivisions, bool invert_normals) {
		obj->array.gen();
		obj->array.bind();
		VBO buffer;
		buffer.gen();
		buffer.bind(GL_ARRAY_BUFFER);
		vector<cube_vert> tris;
		make_sphere_verts(tris, radius, subdivisions, invert_normals);
		buffer.data(GL_ARRAY_BUFFER, tris);
		obj->array.pointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
		obj->array.pointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (const void*)(3*sizeof(float)));
		obj->array.pointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (const void*)(6*sizeof(float)));
		obj->array.unbind();
		obj->nelements = tris.size();
	}


}
