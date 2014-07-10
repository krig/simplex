#pragma once

namespace geo {

	struct cube_vert {
		vec3 pos;
		vec3 normal;
	};

	struct cube {
		cube() : array(0), buffer(0), transform() {
		}

		void make(const vec3& size);

		void render(shader_program* material, const mat4& proj, const mat4& view) {
			material->use();
			material->uniform("projection", proj);
			material->uniform("view", view);
			material->uniform("model", transform);
			bind_vao(array);
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
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

		void render(shader_program* material, const mat4& proj, const mat4& view) {
			material->use();
			material->uniform("projection", proj);
			material->uniform("view", view);
			material->uniform("model", transform);
			bind_vao(array);
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glDrawArrays(GL_TRIANGLES, 0, segments * segments * 6);

		}

		float segment_size;
		int segments;
		GLuint array, buffer;
		mat4 transform;
	};

}
