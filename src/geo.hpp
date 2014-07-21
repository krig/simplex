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

	struct cube_vert {
		vec3 pos;
		vec3 normal;
		vec2 texcoord;
	};

	struct cube {
		cube() : array(0), buffer(0), transform() {
		}

		// create the cube geometry
		void make(const vec3& size, bool skybox = false);

		void render(const mat4& proj, const mat4& view) {
			bind_vao(array);
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glDrawArrays(GL_TRIANGLES, 0, 6 * 2 * 3);

		}

		GLuint array, buffer;
		mat4 transform;
	};

	struct plane {
		plane() : array(0), buffer(0), transform() {
			segment_size = 3.f;
			segments = 100;
		}

		void make();

		void render(Material* material, const mat4& proj, const mat4& view) {
			material->use();
			material->uniform("projection", proj);
			material->uniform("view", view);
			material->uniform("model", transform);
			bind_vao(array);
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glDrawArrays(GL_TRIANGLES, 0, segments * segments * 6);

		}

		float segment_size;
		int segments;
		GLuint array, buffer;
		mat4 transform;
	};

}
