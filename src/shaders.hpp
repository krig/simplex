#pragma once
#include "file.hpp"
#include <memory>

struct shader {
	shader(GLenum type) {
		name = glCreateShader(type);
	}

	bool compile(const char* source) {
		glShaderSource(name, 1, &source, nullptr);
		glCompileShader(name);
		GLint status;
		glGetShaderiv(name, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			GLint length;
			glGetShaderiv(name, GL_INFO_LOG_LENGTH, &length);
			std::vector<GLchar> info(length);
			glGetShaderInfoLog(name, length, nullptr, info.data());
			fprintf(stderr, "glCompileShader failed:\n%s\n", info.data());
		}
		return status != GL_FALSE;
	}

	~shader() {
		glDeleteShader(name);
	}

	GLuint name;
};

struct shader_program {
	shader_program() {
		name = glCreateProgram();
	}

	bool compile(GLuint shader1, GLuint shader2) {
		glAttachShader(name, shader1);
		glAttachShader(name, shader2);
		glLinkProgram(name);
		GLint status;
		glGetProgramiv(name, GL_LINK_STATUS, &status);
		if (status == GL_FALSE) {
			GLint length;
			glGetProgramiv(name, GL_INFO_LOG_LENGTH, &length);
			std::vector<GLchar> info(length);
			glGetProgramInfoLog(name, length, nullptr, info.data());
			fprintf(stderr, "glLinkProgram failed: %s\n", info.data());
		}
		glDetachShader(name, shader1);
		glDetachShader(name, shader2);
		return status != GL_FALSE;
	}

	~shader_program() {
		glDeleteProgram(name);
	}

	void use() {
		glUseProgram(name);
	}

	void uniform(const char* loc, const glm::mat4& m) {
		GLuint index = glGetUniformLocation(name, loc);
		glUniformMatrix4fv(index, 1, GL_FALSE, glm::value_ptr(m));
	}

	void vertex_attrib(const char* loc, const glm::vec3& v) {
		GLuint i = glGetAttribLocation(name, loc);
		glVertexAttrib3f(i, v.x, v.y, v.z);
	}

	void vertex_attrib(const char* loc, const float* p, GLuint num, GLuint stride) {
		GLuint i = glGetAttribLocation(name, loc);
		glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
		glEnableVertexAttribArray(i);
	}

	GLuint name;
};

inline std::unique_ptr<shader_program> load_program(const char* vertex_shader, const char* fragment_shader) {
	shader v(GL_VERTEX_SHADER);
	shader f(GL_FRAGMENT_SHADER);
	string vsh = util::read_file(vertex_shader);
	string fsh = util::read_file(fragment_shader);
	v.compile(vsh.c_str());
	f.compile(fsh.c_str());
	std::unique_ptr<shader_program> program(new shader_program);
	program->compile(v.name, f.name);
	return program;
}

GLuint make_vao() {
	GLuint id;
	glGenVertexArrays(1, &id);
	return id;
}

void bind_vao(GLuint vao) {
	glBindVertexArray(vao);
}

GLuint make_vbo() {
	GLuint id;
	glGenBuffers(1, &id);
	return id;
}

template <class V>
void fill_static_vbo(GLuint vbo, const V& v) {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(v[0]), (const void*)v.data(), GL_STATIC_DRAW);
}


