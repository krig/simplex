#pragma once

struct Mesh;
struct Material;
struct Texture;


// mesh -> vertex data
struct Mesh {
	GLuint vao;
	GLuint vbo;
};


// material -> shaders, texture, animated texture, multitexture
struct Material {
	// ...

	~Material() {
		glDeleteProgram(program);
	}

	void use() {
		glUseProgram(program);
	}

	void uniform(const char* loc, const vec3& v) {
		GLuint index = glGetUniformLocation(program, loc);
		glUniform3fv(index, 1, glm::value_ptr(v));
	}

	void uniform(const char* loc, const mat4& m) {
		GLuint index = glGetUniformLocation(program, loc);
		glUniformMatrix4fv(index, 1, GL_FALSE, glm::value_ptr(m));
	}

	void vertex_attrib(const char* loc, const vec3& v) {
		GLuint i = glGetAttribLocation(program, loc);
		glVertexAttrib3f(i, v.x, v.y, v.z);
	}

	void vertex_attrib(const char* loc, const float* p, GLuint num, GLuint stride) {
		GLuint i = glGetAttribLocation(program, loc);
		glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
		glEnableVertexAttribArray(i);
	}

	GLuint program;
	Material* next;
};

struct Texture {
	// ...
};

Texture* load_texture(const char* name);
Material* load_material(const char* name);
void release_assets();

// material <name>
// vertexshader <name>
// fragmentshader <name>
// inputs {
// 3 float vertex
// 3 float normal
// 2 float texcoord
// }

