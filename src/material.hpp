#pragma once

// material -> shaders, texture, animated texture, multitexture
// also information like solidity, translucency, step sound, break sound, break material...
struct Material : public Asset {
	// ...

	~Material() {
		glDeleteProgram(program);
	}

	void use() {
		glUseProgram(program);
	}

	void uniform(const char* loc, const vec2& v) {
		GLuint index = glGetUniformLocation(program, loc);
		glUniform2fv(index, 1, glm::value_ptr(v));
	}

	void uniform(const char* loc, const vec3& v) {
		GLuint index = glGetUniformLocation(program, loc);
		glUniform3fv(index, 1, glm::value_ptr(v));
	}

	void uniform(const char* loc, const vec4& v) {
		GLuint index = glGetUniformLocation(program, loc);
		glUniform4fv(index, 1, glm::value_ptr(v));
	}

	void uniform(const char* loc, const mat3& m) {
		GLuint index = glGetUniformLocation(program, loc);
		glUniformMatrix3fv(index, 1, GL_FALSE, glm::value_ptr(m));
	}

	void uniform(const char* loc, const mat4& m) {
		GLuint index = glGetUniformLocation(program, loc);
		glUniformMatrix4fv(index, 1, GL_FALSE, glm::value_ptr(m));
	}

	void uniform(const char* loc, int i) {
		GLuint index = glGetUniformLocation(program, loc);
		glUniform1i(index, i);
	}

	void attrib(const char* loc, const vec3& v) {
		GLuint i = glGetAttribLocation(program, loc);
		glVertexAttrib3f(i, v.x, v.y, v.z);
	}

	void attrib(const char* loc, const float* p, GLuint num, GLuint stride) {
		GLuint i = glGetAttribLocation(program, loc);
		glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
		glEnableVertexAttribArray(i);
	}

	GLuint attrib(const char* loc) {
		return glGetAttribLocation(program, loc);
	}

	void load();

	GLuint program;
	string vshader;
	string fshader;
};

Material* load_material(const char* name);
