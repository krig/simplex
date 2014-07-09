#pragma once

namespace geo {

struct cube_vert {
	vec3 pos;
	vec3 normal;
};

std::vector<cube_vert> make_cube_vertices(const vec3& size);


struct cube {
	cube() : array(0), buffer(0), transform() {
	}

	void make(const vec3& size) {
		array = make_vao();
		bind_vao(array);
		buffer = make_buffer(GL_ARRAY_BUFFER, make_cube_vertices(size));
		glVertexAttribPointer(0u, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), 0);
		glVertexAttribPointer(1u, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (const void*)(3*sizeof(float)));
		glVertexAttrib3f(2u, 1.f, 1.f, 1.f);
	}

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

	void set_color(const vec3& clr) {
		bind_vao(array);
		glVertexAttrib3f(2u, clr.x, clr.y, clr.z);
	}

	GLuint array, buffer;
	mat4 transform;
};

}
