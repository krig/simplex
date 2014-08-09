#pragma once

#include "assets.hpp"

namespace geo {

	// a tesselator which produces vertex data
	// optionally add normals, texcoords, colors
	// or other per-vertex data
	// allow access to vertex data directly or
	// upload to opengl and return a VAO name

	// something which holds a VAO and information
	// about how to render it with a given material

	// icosahedron:
	// (0, +-1, +-tau)
	// (+-1, +-tau, 0)
	// (+-tau, 0, +-1)

	// dodecahedron
	// (+-1, +-1, +-1)
	// (0, +-1/tau, +-tau)
	// (+-1/tau, +-tau, 0)
	// (+-tau, 0, +-1/tau)

	struct cube_vert {
		vec3 pos;
		vec3 normal;
		vec2 texcoord;
	};

	struct geometry {
		geometry() : array(), nelements(0) {}

		void render() {
			array.bind();
			glDrawArrays(GL_TRIANGLES, 0, nelements);
			array.unbind();
		}

		VAO array;
		size_t nelements;
	};

	struct cube : public geometry {
		cube() : geometry(), transform() {}
		void make(const vec3& size, bool invert_normals = false);
		mat4 transform;
	};

	struct cone : public geometry {
		cone() : geometry(), transform() {}
		void make(float height, float radius, int subdivisions = 6, bool invert_normals = false);
		mat4 transform;
	};

	struct plane : public geometry {
		plane() : geometry(), transform() {}
		void make(float segment_size = 3.f, int segments = 100);
		mat4 transform;
	};

}
