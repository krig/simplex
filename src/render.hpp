#pragma once
// render-related stuff

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

namespace gl {
	struct vertex_array {
		vertex_array() : name(0) {
		}

		~vertex_array() {
			if (valid())
				glDeleteVertexArrays(1, &name);
		}

		void gen() {
			glGenVertexArrays(1, &name);
		}

		void bind() {
			glBindVertexArray(name);
		}

		bool valid() const {
			return glIsVertexArray(name);
		}

		operator GLuint () { return name; }

		GLuint name;
	};

	struct buffer {
		buffer() : name(0) {
		}

		~buffer() {
			if (valid()) {
				glDeleteBuffers(1, &name);
			}
		}

		void gen() {
			glGenBuffers(1, &name);
		}

		void bind(GLenum target) {
			glBindBuffer(target, name);
		}

		bool valid() const {
			return glIsBuffer(name);
		}

		operator GLuint () { return name; }

		GLuint name;
	};
}

// something to render
struct renderobj {
	shader_program* material;
	mat4 transform;


	std::vector<GLuint> buffers;
	std::vector<GLuint> arrays;
};
