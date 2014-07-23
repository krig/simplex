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

	struct cube {
		cube() {
		}

		// create the cube geometry
		void make(const vec3& size, bool skybox = false);

		void render() {
			array.bind();
			glDrawArrays(GL_TRIANGLES, 0, 6 * 2 * 3);
			array.unbind();
		}

		VAO array;
		VBO buffer;
		mat4 transform;
	};

	struct plane {
		plane() {
			segment_size = 3.f;
			segments = 100;
		}

		void make();

		void render() {
			array.bind();
			glDrawArrays(GL_TRIANGLES, 0, segments * segments * 6);
			array.unbind();
		}

		float segment_size;
		int segments;
		VAO array;
		VBO buffer;
		mat4 transform;
	};

}
